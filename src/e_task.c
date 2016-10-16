#include "search.h"
#include "task.h"

int main(int argc, char** argv) {
  TaskResult *result;
  int* done;
  done = (int*)(DONE_ADDR);
  result = (TaskResult*)(CORE_ADDR);
  task(result);
  *done = 1;

  //Put core in idle state
  __asm__ __volatile__("idle");
}
