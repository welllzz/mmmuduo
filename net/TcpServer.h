
#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include "TcpConnection.h"
#include "Declear.h"
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>



class TcpServer : boost::noncopyable
{
 public:
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void (const TcpConnectionPtr&,Buffer*)> MessageCallback;
    typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
    typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

  TcpServer(EventLoop* loop);
  ~TcpServer();  // force out-line dtor, for scoped_ptr members.

  void setThreadNum(int numThreads);
  void setThreadInitCallback(const ThreadInitCallback& cb)
  { _threadInitCallback = cb; }

  void start();
  // 设置连接到来或者连接关闭回调函数
  void setConnectionCallback(const ConnectionCallback& cb)
  { _connectionCallback = cb; }
  // 设置消息到来回调函数
  void setMessageCallback(const MessageCallback& cb)
  { _messageCallback = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { _writeCompleteCallback = cb; }


 private:
  void newConnection(int sockfd);
  
  typedef std::map<int, TcpConnectionPtr> ConnectionMap;

  EventLoop* _loop;  // the acceptor loop

  boost::scoped_ptr<Acceptor> _acceptor; // avoid revealing Acceptor
  boost::scoped_ptr<EventLoopThreadPool> _threadPool;
  ConnectionCallback _connectionCallback;
  MessageCallback _messageCallback;
  WriteCompleteCallback _writeCompleteCallback;		// 数据发送完毕，会回调此函数
  ThreadInitCallback _threadInitCallback;	// IO线程池中的线程在进入事件循环前，会回调用此函数
  bool _started;
  // always in loop thread
  int _nextConnId;				// 下一个连接ID
  ConnectionMap _connections;	// 连接列表
};


#endif  // MUDUO_NET_TCPSERVER_H
