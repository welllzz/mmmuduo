
#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

#include <boost/bind.hpp>

#include <stdio.h>  // snprintf



TcpServer::TcpServer(EventLoop* loop)
  : _loop(loop),
    _acceptor(new Acceptor(loop)),
    _threadPool(new EventLoopThreadPool(loop)),
    _started(false),
    _nextConnId(1)
{
  // Acceptor::handleRead函数中会回调用TcpServer::newConnection
  // _1对应的是socket文件描述符，_2对应的是对等方的地址(InetAddress)
  _acceptor->setNewConnectionCallback(
      boost::bind(&TcpServer::newConnection, this));
}

TcpServer::~TcpServer()
{
}

void TcpServer::setThreadNum(int numThreads)
{
  assert(0 <= numThreads);
  _threadPool->setThreadNum(numThreads);
}

// 该函数多次调用是无害的
// 该函数可以跨线程调用
void TcpServer::start()
{
  if (!_started)
  {
    _started = true;
	_threadPool->start(_threadInitCallback);
  }

  if (!_acceptor->listenning())
  {
	// get_pointer返回原生指针
    _loop->runInLoop(
        boost::bind(&Acceptor::listen, get_pointer(_acceptor)));
  }
}

void TcpServer::newConnection(int sockfd)
{
  // 按照轮叫的方式选择一个EventLoop
    EventLoop* ioLoop = _threadPool->getNextLoop();
    ++_nextConnId;
    TcpConnectionPtr conn(new TcpConnection(sockfd,ioLoop));

    _connections[_nextConnId] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);
    ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn));

}



