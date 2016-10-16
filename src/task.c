#ifdef __cplusplus
extern "C" {
#endif

#include "task.h"
#include "score.h"

TaskResult task(TaskResult oldResult, int i) {
  int score = oldResult.score;
  int alpha, beta;
  int previousAlpha = oldResult.alpha;
  int previousBeta = oldResult.beta;
  int delta = oldResult.delta;
  const void* move = oldResult.move;
  bool inCheck = oldResult.inCheck;
  bool quietMove = oldResult.quietMove;
  bool searchNeedsMoreTime = oldResult.searchNeedsMoreTime;
  void* bestMove = oldResult.bestMove;
  const bool lowerBound = score >= oldResult.beta;
  if (lowerBound) {
    searchNeedsMoreTime = i > 0;
    bestMove = (void*)move;
    beta = infinity;
    if (!isWinScore(score) && previousBeta + delta < beta) {
      beta = previousBeta + delta;
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
    searchNeedsMoreTime = true;
    alpha = -infinity;
    if (!isLoseScore(score) && previousAlpha - delta < alpha) {
      alpha = previousAlpha - delta;
    }
  }
  return (TaskResult){ score, alpha, beta, delta, move, bestMove,
      inCheck, quietMove, searchNeedsMoreTime
      };
}

#ifdef __cplusplus
}
#endif
