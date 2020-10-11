
#include "EventLoopThreadPool.h"

#include "EventLoop.h"
#include "EventLoopThread.h"

#include <boost/bind.hpp>



EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
  : _baseLoop(baseLoop),
    _started(false),
    _numThreads(0),
    _next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
  _started = true;

  for (int i = 0; i < _numThreads; ++i)
  {
    EventLoopThread* t = new EventLoopThread(cb);
    _threads.push_back(t);
    _loops.push_back(t->startLoop());	// 启动EventLoopThread线程，在进入事件循环之前，会调用cb
  }
  if (_numThreads == 0 && cb)
  {
    // 只有一个EventLoop，在这个EventLoop进入事件循环之前，调用cb
    cb(_baseLoop);
  }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  EventLoop* loop = _baseLoop;

  // 如果loops_为空，则loop指向baseLoop_
  // 如果不为空，按照round-robin（RR，轮叫）的调度方式选择一个EventLoop
  if (!_loops.empty())
  {
    // round-robin
    loop = _loops[_next];
    ++_next;
    if ((_next) >= static_cast<int>(_loops.size()))
    {
      _next = 0;
    }
  }
  return loop;
}

