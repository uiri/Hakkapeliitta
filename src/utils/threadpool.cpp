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

static void waitOnDone(e_epiphany_t *dev, int r, int c) {
  int done;

  do {
    e_read(dev, r, c, DONE_ADDR, &done, sizeof(done));
  } while (!done);
}

TaskResult ThreadPool::joinEpiphanyJob(int c_id) {
  TaskResult tr;

  waitOnDone(dev, c_id/4, c_id%4);
  e_read(dev, c_id/4, c_id%4, CORE_ADDR, &tr, sizeof(tr));
  return tr;
}

int ThreadPool::addEpiphanyJob(TaskResult result) {
  int row, col, done, c_id;
  c_id = core_id;
  core_id = (core_id+1)%16;
  row = c_id/4;
  col = c_id%4;

  waitOnDone(dev, row, col);

  done = 0;
  e_write(dev, row, col, DONE_ADDR, &done, sizeof(done));
  e_write(dev, row, col, CORE_ADDR, &result, sizeof(TaskResult));
  e_start(dev, row, col);

  return c_id;
}
