/*
    Hakkapeliitta - A UCI chess engine. Copyright (C) 2013-2015 Mikko Aarnos.

    Hakkapeliitta is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Hakkapeliitta is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Hakkapeliitta. If not, see <http://www.gnu.org/licenses/>.
*/

#include "uci.hpp"
#include <iostream>
#include "utils/clamp.hpp"
#include "benchmark.hpp"
#include "search_parameters.hpp"
#include "textio.hpp"
#include "syzygy/tbprobe.hpp"
#include "utils/threadpool.hpp"
#include "score.h"
#include "utils/epiphany.h"

UCI::UCI() :
search(*this), sync_cout(std::cout), ponder(true),
contempt(0), pawnHashTableSize(4), transpositionTableSize(32), syzygyProbeDepth(1), 
syzygyProbeLimit(6), syzygy50MoveRule(true), rootPly(0)
{
    addCommand("uci", &UCI::sendInformation);
    addCommand("isready", &UCI::isReady);
    addCommand("stop", &UCI::stop);
    addCommand("setoption", &UCI::setOption);
    addCommand("ucinewgame", &UCI::newGame);
    addCommand("position", &UCI::position);
    addCommand("go", &UCI::go);
    addCommand("quit", &UCI::quit);
    addCommand("ponderhit", &UCI::ponderhit);
    addCommand("displayboard", &UCI::displayBoard);
    addCommand("perft", &UCI::perft);

    repetitionHashKeys.assign(1024, 0);
}

void UCI::mainLoop()
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::string cmd;

    for (;;)
    {
        // Read a line from stdin.
        if (!std::getline(std::cin, cmd))
            break;

        // Using string stream makes parsing a lot easier than regexes.
        // Note to self: Lucas Braesch is always right.
        std::istringstream iss(cmd);
        std::string commandName;
        iss >> commandName;
        
        // Ignore empty lines.
        if (commandName.empty())
            continue;

        // If we are currently searching only the commands stop, quit, and isready are legal.
        if (search.isSearching() && commandName != "stop" && commandName != "quit" && commandName != "isready" && commandName != "ponderhit")
        {
            continue;
        }

        // Go through the list of commands and call the correct function if the command entered is known.
        // If the command is unknown report that.
        if (commands.count(commandName))
        {
            (this->*commands[commandName])(pos, iss);
        }
        else
        {
            sync_cout << "info string unknown command" << std::endl;
        }
    }
}

void UCI::addCommand(const std::string& name, FunctionPointer fp)
{
    commands[name] = fp;
}

// UCI commands.

void UCI::sendInformation(Position&, std::istringstream&)
{
    // Send the name of the engine and the name of it's author.
    sync_cout << "id name Hakkapeliitta 3.0" << std::endl;
    sync_cout << "id author Mikko Aarnos" << std::endl;

    // Send all possible options the engine has that can be modified.
    sync_cout << "option name Hash type spin default 32 min 1 max 65536" << std::endl;
    sync_cout << "option name Pawn Hash type spin default 4 min 1 max 8192" << std::endl;
    sync_cout << "option name Clear Hash type button" << std::endl;
    sync_cout << "option name Contempt type spin default 0 min -75 max 75" << std::endl;
    sync_cout << "option name Ponder type check default true" << std::endl;
    sync_cout << "option name SyzygyPath type string default <empty>" << std::endl;
    sync_cout << "option name SyzygyProbeDepth type spin default 1 min 1 max 100" << std::endl;
    sync_cout << "option name SyzygyProbeLimit type spin default 6 min 0 max 6" << std::endl;
    sync_cout << "option name Syzygy50MoveRule type check default true" << std::endl;

    // Send a response telling the listener that we are ready in UCI-mode.
    sync_cout << "uciok" << std::endl;
}

void UCI::isReady(Position&, std::istringstream&)
{
    sync_cout << "readyok" << std::endl;
}

void UCI::stop(Position&, std::istringstream&)
{
    search.stopPondering();
    search.stopSearching();
}

void UCI::quit(Position&, std::istringstream&)
{
    search.stopPondering();
    search.stopSearching();
    // TODO: it might be cleaner to just exit the mainLoop somehow instead of this.
    exit(0);
}

void UCI::setOption(Position&, std::istringstream& iss) 
{
    std::string name, s;
    
    iss >> s; // Get rid of the "value" in front.

    // Read the name of the option. 
    // Since the name can contains spaces we have this loop.
    while (iss >> s && s != "value")
    {
        name += std::string(" ", !name.empty()) + s;
    }

    if (name == "Contempt")
    {
        iss >> contempt;
        contempt = clamp(contempt, -75, 75);
    }
    else if (name == "Hash")
    {
        iss >> transpositionTableSize;
        search.setTranspositionTableSize(transpositionTableSize);
    }
    else if (name == "Pawn Hash")
    {
        iss >> pawnHashTableSize;
        search.setPawnHashTableSize(pawnHashTableSize);
    }
    else if (name == "Clear Hash")
    {
        search.clearSearch();
    }
    else if (name == "Ponder")
    {
        iss >> ponder;
    }
    else if (name == "SyzygyPath")
    {
        std::string path;
        while (iss >> s)
        {
            path += std::string(" ", !path.empty()) + s;
        }
        Syzygy::initialize(path);
    }
    else if (name == "SyzygyProbeDepth")
    {
        iss >> syzygyProbeDepth;
        syzygyProbeDepth = clamp(syzygyProbeDepth, 1, 100);
    }
    else if (name == "SyzygyProbeLimit")
    {
        iss >> syzygyProbeLimit;
        syzygyProbeLimit = clamp(syzygyProbeLimit, 0, 6);
    }
    else if (name == "Syzygy50MoveRule")
    {
        iss >> std::boolalpha >> syzygy50MoveRule;
    }
    else
    {
        sync_cout << "info string no such option exists" << std::endl;
    }
}

void UCI::newGame(Position&, std::istringstream&)
{
    search.clearSearch();
}

void UCI::go(Position& pos, std::istringstream& iss)
{
    SearchParameters searchParameters;
    std::string s;

    // Parse the string to get the parameters for the search.
    while (iss >> s)
    {
        if (s == "searchmoves") {} // TODO: implement this
        else if (s == "ponder") { searchParameters.mPonder = true; }
        else if (s == "wtime") { iss >> searchParameters.mTime[Color::White]; }
        else if (s == "btime") { iss >> searchParameters.mTime[Color::Black]; }
        else if (s == "winc") { iss >> searchParameters.mIncrement[Color::White]; }
        else if (s == "binc") { iss >> searchParameters.mIncrement[Color::Black]; }
        else if (s == "movestogo") { iss >> searchParameters.mMovesToGo; searchParameters.mMovesToGo += 2; }
        else if (s == "depth") { iss >> searchParameters.mDepth; }
        else if (s == "nodes") { iss >> searchParameters.mNodes; }
        else if (s == "mate") { searchParameters.mMate = true; }
        else if (s == "movetime") { iss >> searchParameters.mMoveTime; }
        else if (s == "infinite") { searchParameters.mInfinite = true; }
    }

    searchParameters.mRootPly = rootPly;
    searchParameters.mHashKeys = repetitionHashKeys;
    searchParameters.mSyzygyProbeDepth = syzygyProbeDepth;
    searchParameters.mSyzygyProbeLimit = syzygyProbeLimit;
    searchParameters.mSyzygy50MoveRule = syzygy50MoveRule;

    search.go(pos, searchParameters);
}

void UCI::position(Position& pos, std::istringstream& iss)
{
    std::string s, fen;
    rootPly = 0;

    iss >> s;
    
    if (s == "startpos")
    {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        iss >> s; // Get rid of "moves" which should follow this
    }
    else if (s == "fen")
    {
        while (iss >> s && s != "moves")
        {
            fen += s + " ";
        }
    }
    else
    {
        return;
    }

    pos = Position(fen);

    // Parse the moves.
    while (iss >> s)
    {
        Piece promotion = Piece::Empty;
        const auto from = (s[0] - 'a') + 8 * (s[1] - '1');
        const auto to = (s[2] - 'a') + 8 * (s[3] - '1');

        if (s.size() == 5)
        {
            switch (s[4])
            {
                case 'q': promotion = Piece::Queen; break;
                case 'r':promotion = Piece::Rook; break;
                case 'b': promotion = Piece::Bishop; break;
                case 'n': promotion = Piece::Knight; break;
                default:;
            }
        }
        else if (pos.getBoard(from).getPieceType() == Piece::King && std::abs(from - to) == 2)
        {
            promotion = Piece::King;
        }
        else if (pos.getBoard(from).getPieceType() == Piece::Pawn && to == pos.getEnPassantSquare())
        {
            promotion = Piece::Pawn;
        }

        repetitionHashKeys[rootPly++] = pos.getHashKey();
        pos.makeMove(Move(from, to, promotion));
    }
}

void UCI::ponderhit(Position&, std::istringstream&)
{
    search.stopPondering();
}

void UCI::displayBoard(Position& pos, std::istringstream&)
{
    sync_cout << pos << std::endl; 
}

void UCI::perft(Position& pos, std::istringstream& iss)
{
    int depth;

    if (iss >> depth)
    {
        const auto result = Benchmark::runPerft(pos, depth);
        sync_cout << "info string nodes " << result.first
                  << " time " << result.second
                  << " nps " << (result.first / (result.second + 1)) * 1000 << std::endl;
    }
    else
    {
        sync_cout << "info string argument invalid" << std::endl;
    }
}

void UCI::infoCurrMove(const Move& move, int depth, int nr)
{
    sync_cout << "info depth " << depth
              << " currmove " << moveToUciFormat(move)
              << " currmovenumber " << nr + 1 << std::endl;
}

void UCI::infoRegular(uint64_t nodeCount, uint64_t tbHits, uint64_t searchTime)
{
    sync_cout << "info nodes " << nodeCount
              << " time " << searchTime
              << " nps " << (nodeCount / (searchTime + 1)) * 1000
              << " tbhits " << tbHits << std::endl;
}

void UCI::infoPv(const std::vector<Move>& pv, uint64_t searchTime,
                 uint64_t nodeCount, uint64_t tbHits,
                 int depth, int score, int flags, int selDepth)
{
    std::stringstream ss;

    ss << "info depth " << depth << " seldepth " << selDepth;
    if (isMateScore(score))
    {
        score = (score > 0 ? ((mateScore - score + 1) >> 1) : ((-score - mateScore) >> 1));
        ss << " score mate " << score;
    }
    else
    {
        ss << " score cp " << score;
    }

    if (flags == TranspositionTable::Flags::LowerBoundScore)
    {
        ss << " lowerbound";
    }
    else if (flags == TranspositionTable::Flags::UpperBoundScore)
    {
        ss << " upperbound";
    }

    ss << " time " << searchTime
       << " nodes " << nodeCount
       << " nps " << (nodeCount / (searchTime + 1)) * 1000
       << " tbhits " << tbHits
       << " pv " << movesToUciFormat(pv) << std::endl;

    sync_cout << ss.str();
}

void UCI::infoBestMove(const std::vector<Move>& pv, uint64_t searchTime, 
                       uint64_t nodeCount, uint64_t tbHits)
{
    sync_cout << "info time " << searchTime
              << " nodes " << nodeCount
              << " nps " << (nodeCount / (searchTime + 1)) * 1000
              << " tbhits " << tbHits << std::endl
              << "bestmove " << moveToUciFormat(pv[0])
              << " ponder " << (pv.size() > 1 ? moveToUciFormat(pv[1]) : "(none)") << std::endl;
}
