
#ifndef MUDUO_NET_EVENTLOOPTHREADPOOL_H
#define MUDUO_NET_EVENTLOOPTHREADPOOL_H

#include "../base/Condition.h"
#include "../base/Mutex.h"

#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "Declear.h"


class EventLoopThreadPool : boost::noncopyable
{
 public:
  typedef boost::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThreadPool(EventLoop* baseLoop);
  ~EventLoopThreadPool();
  void setThreadNum(int numThreads) { _numThreads = numThreads; }
  void start(const ThreadInitCallback& cb = ThreadInitCallback());
  EventLoop* getNextLoop();

 private:

  EventLoop* _baseLoop;	// 与Acceptor所属EventLoop相同
  bool _started;
  int _numThreads;		// 线程数
  int _next;			// 新连接到来，所选择的EventLoop对象下标
  boost::ptr_vector<EventLoopThread> _threads;		// IO线程列表
  std::vector<EventLoop*> _loops;					// EventLoop列表
};



#endif  // MUDUO_NET_EVENTLOOPTHREADPOOL_H
