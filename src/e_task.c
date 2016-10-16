#include "search.h"
#include "task.h"

int main(int argc, char** argv) {
  TaskResult *result;
  int* done;
  done = (int*)(DONE_ADDR);
  result = (TaskResult*)(CORE_ADDR);
  *done = 1;
  task(result);

  return 0;
}
