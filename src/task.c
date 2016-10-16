#ifdef __cplusplus
extern "C" {
#endif

#include "task.h"
#include "score.h"
#include "search.h"

  // From TT Flags
  #define FLAG_EMPTY 0
  #define FLAG_EXACT 1
  #define FLAG_UPPER 2
  #define FLAG_LOWER 3

void task(TaskResult *result) {
  int i = result->i;
  int score = result->score;
  int alpha = result->alpha;
  int beta = result->beta;
  int delta = result->delta;
  const bool lowerBound = score >= beta;
  result->searchNeedsMoreTime = lowerBound ? i > 0 : true;
  if (lowerBound) {
    result->bestMove = result->move;
    result->beta = infinity;
    if (!isWinScore(score) && beta + delta < result->beta) {
      result->beta = beta + delta;
    }
  } else {
    result->alpha = -infinity;
    if (!isLoseScore(score) && result->alpha < alpha - delta) {
      result->alpha = alpha - delta;
    }
  }
}

#ifdef __cplusplus
}
#endif
