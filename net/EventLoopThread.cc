#include "EventLoopThread.h"
#include "EventLoop.h"
#include <boost/bind.hpp>


EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
  : _loop(NULL),
    _exiting(false),
    _thread(boost::bind(&EventLoopThread::threadFunc, this)),
    _mutex(),
    _cond(_mutex),
    _callback(cb)
{
}

EventLoopThread::~EventLoopThread()
{
  _exiting = true;
  _loop->quit();		// 退出IO线程，让IO线程的loop循环退出，从而退出了IO线程
  _thread.join();
}

EventLoop* EventLoopThread::startLoop()
{
  _thread.start();

  {
    MutexLockGuard lock(_mutex);
    while (_loop == NULL)
    {
      _cond.wait();
    }
  }

  return _loop;
}

void EventLoopThread::threadFunc()
{
  EventLoop loop;

  if (_callback)
  {
    _callback(&loop);
  }

  {
    MutexLockGuard lock(_mutex);
    // loop_指针指向了一个栈上的对象，threadFunc函数退出之后，这个指针就失效了
    // threadFunc函数退出，就意味着线程退出了，EventLoopThread对象也就没有存在的价值了。
    // 因而不会有什么大的问题
    _loop = &loop;
    _cond.notify();
  }

  loop.loop();//loop会一直循环，直到 析构函数中调用loop->quit，退出后，该线程也退出
  //assert(exiting_);
}

