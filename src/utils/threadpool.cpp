#include "../move.hpp"
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

void ThreadPool::addEpiphanyJob(Move move) {
  int row, col, done;
  row = core_id/4;
  col = core_id%4;
  do {
    e_read(dev, row, col, 0x2000, &done, sizeof(done));
  } while (!done);

  e_write(dev, row, col, 0x2008, &move, sizeof(Move));
  e_start(dev, row, col);

  core_id++;
}
