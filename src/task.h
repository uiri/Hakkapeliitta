#ifndef TASK_H_
#define TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <math.h>
#include <stdbool.h>

typedef struct task_result TaskResult;
struct task_result {
  int score;
  int alpha;
  int beta;
  int delta;
  const void* move;
  void* bestMove;
  bool inCheck;
  bool quietMove;
  bool searchNeedsMoreTime;
  bool lowerBound;
};

TaskResult task(TaskResult oldResult, int i);

#ifdef __cplusplus
}
#endif

#endif
