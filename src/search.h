#ifndef SEARCH_H_
#define SEARCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define DONE_ADDR 0x2000
#define CORE_ADDR 0x2010
  
typedef struct TranspositionTable TranspositionTable;
typedef uint64_t HashKey;
typedef struct Move Move;
typedef struct Search Search;
typedef struct SearchListener SearchListener;
typedef struct Position Position;
typedef struct SearchStack SearchStack;

typedef struct task_result TaskResult;
struct task_result {
  int i;
  int score;
  int alpha;
  int beta;
  int delta;
  const Move* move;
  const Move* bestMove;
  bool searchNeedsMoreTime;
};

int ttScoreToRealScore(int score, int ply);
int realScoreToTtScore(int score, int ply);

int task_new_search(Search* search, int newDepth, Position newPosition,
		    int givesCheck, SearchStack* ss, TaskResult* result);


#ifdef __cplusplus
}
#endif

#endif
