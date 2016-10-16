#ifndef SCORE_H_
#define SCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <float.h>

/// @brief Same value as mateScore in constants.hpp
#define MATE_SCORE 32767

/// @brief The absolute maximum score used in the search function.
const int infinity = MATE_SCORE + 1;

/// @brief The score given to a mate in 500 moves. We do not recognize worse mates, hence the name.
const int minMateScore = 32767 - 1000;

/// @brief Returns true if a given score is "mate in X".
#ifdef __cplusplus
  inline
#endif
bool isWinScore(int score)
{
    assert(score > -infinity && score < infinity);
    return score >= minMateScore;
}

/// @brief Returns true if a given score is "mated in X".
#ifdef __cplusplus
  inline
#endif
bool isLoseScore(int score)
{
    assert(score > -infinity && score < infinity);
    return score <= -minMateScore;
}

/// @brief Returns true if the score is some kind of a mate score.
inline bool isMateScore(int score)
{
    assert(score > -infinity && score < infinity);
    return isWinScore(score) || isLoseScore(score);
}

#ifdef __cplusplus
}
#endif

#endif
