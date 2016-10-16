#ifndef TASK_H_
#define TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>

#include "search.h"

TaskResult task(TaskResult oldResult, int i);

#ifdef __cplusplus
}
#endif

#endif
