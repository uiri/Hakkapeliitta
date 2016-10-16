#include "../search.h"
#include "threadpool.hpp"
#include "epiphany.h"

ThreadPool::ThreadPool(int amountOfThreads) :
  terminateFlag(false),
  core_id(0)
{
    for (auto i = 0; i < amountOfThreads; ++i)
    {
        threads.emplace_back(&ThreadPool::loop, this);
    }
    dev = init_epiphany_threadpool();
}

ThreadPool::~ThreadPool()
{
    terminateFlag = true;
    cv.notify_all();
    for (auto& thread : threads)
    {
        thread.join();
    }
    cleanup_epiphany_threadpool(dev);
}

TaskResult ThreadPool::joinEpiphanyJob(int c_id) {
  TaskResult tr;
  int done;
  off_t addr;

  do {
    e_read(dev, c_id/4, c_id%4, 0x0, &done, sizeof(done));
  } while (!done);

  addr += sizeof(done);
  e_read(dev, c_id/4, c_id%4, 0x0, &tr, sizeof(tr));
  return tr;
}

int ThreadPool::addEpiphanyJob(TaskResult result) {
  int row, col, done;
  off_t addr;
  addr = 0x0;
  row = core_id/4;
  col = core_id%4;

  done = 0;
  e_write(dev, row, col, addr, &done, sizeof(done));
  addr += sizeof(done);
  e_write(dev, row, col, addr, &result, sizeof(TaskResult));
  e_start(dev, row, col);

  return core_id++;
}
