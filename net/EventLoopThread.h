
#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

#include <boost/noncopyable.hpp>


class EventLoop;

class EventLoopThread : boost::noncopyable
{
 public:
  typedef boost::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
  ~EventLoopThread();
  EventLoop* startLoop();	// 启动线程，该线程就成为了IO线程

 private:
  void threadFunc();		// 线程函数

  EventLoop* _loop;			// loop_指针指向一个EventLoop对象
  bool _exiting;
  Thread _thread;
  MutexLock _mutex;
  Condition _cond;
  ThreadInitCallback _callback;		// 回调函数在EventLoop::loop事件循环之前被调用
};



#endif  // MUDUO_NET_EVENTLOOPTHREAD_H

