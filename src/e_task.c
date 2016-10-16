#include "task.h"

int main(int argc, char** argv) {
  TaskResult *result;
  int* done;
  done = (int*)(0x100);
  result = (TaskResult*)(0x110);
  task(result);
  *done = 1;

  return 0;
}
