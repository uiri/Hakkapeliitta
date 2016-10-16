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
  bool inCheck = result->inCheck;
  bool quietMove = result->quietMove;
  const bool lowerBound = score >= beta;
  result->searchNeedsMoreTime = lowerBound ? i > 0 : true;
  if (lowerBound) {
    result->bestMove = result->move;
    result->beta = infinity;
    if (!isWinScore(score) && beta + delta < result->beta) {
      result->beta = beta + delta;
    }
    // Don't forget to update history and killer tables.
    if (!inCheck) {
      /* if (quietMove) { */
      /* 	historyTable.addCutoff(pos, move, depth); */
      /* 	killerTable.update(move, 0); */
      /* } */
      /* for (int j = 0; j < i; ++j) { */
      /* 	const Move move2 = rootMoveList.getMove(j); */
      /* 	if (!pos.captureOrPromotion(move2)) { */
      /* 	  historyTable.addNotCutoff(pos, move2, depth); */
      /* 	} */
      /* } */
    }
  } else {
    result->alpha = -infinity;
    if (!isLoseScore(score) && alpha - delta < result->alpha) {
      result->alpha = alpha - delta;
    }
  }
}

#ifdef __cplusplus
}
#endif
