#ifndef MAIN_CPP
#define MAIN_CPP

#include "defs.h"
#include "bitboard.h"
#include "magic.h"
#include "hash.h"
#include "ttable.h"
#include "position.h"
#include "search.h"
#include "time.h"

int main()
{
	cout << "Hakkapeliitta v2, (C) 2013-2014 Mikko Aarnos" << endl;

	initializeBitboards();
	initializeMagics();
	initializeHash();

	Position pos;
	// pos.initializeBoardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	pos.initializeBoardFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
	// pos.initializeBoardFromFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
	// pos.initializeBoardFromFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

	Timer t;
	t.start();
	perft(3, pos);
	t.stop();

	cout << nodeCount << endl;
	cout << t.getms() << endl;

	cout << "done" << endl;

	return 0; 
}

#endif