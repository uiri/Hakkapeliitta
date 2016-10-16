#ifndef SEARCH_H_
#define SEARCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct TranspositionTable TranspositionTable;
typedef uint64_t HashKey;
typedef struct Move Move;
typedef struct Search Search;
typedef struct SearchListener SearchListener;
typedef struct Position Position;
typedef struct SearchStack SearchStack;

typedef struct task_result TaskResult;
struct task_result {
  int score;
  int alpha;
  int beta;
  int delta;
  const Move* move;
  const Move* bestMove;
  bool inCheck;
  bool quietMove;
  bool searchNeedsMoreTime;
  bool lowerBound;
};

int ttScoreToRealScore(int score, int ply);
int realScoreToTtScore(int score, int ply);

int task_new_search(Search* search, int newDepth, Position newPosition,
		    int givesCheck, SearchStack* ss, TaskResult* result);


#ifdef __cplusplus
}
#endif

#endif
