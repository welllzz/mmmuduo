//author voidccc

#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Define.h"

#include <boost/bind.hpp>
#include <string.h> //for bzero
#include <iostream>
using namespace std;

TcpConnection::TcpConnection(int sockfd, EventLoop* pLoop)
    :_sockfd(sockfd)
    ,_ploop(pLoop)
    ,_pChannel(new Channel(_ploop,_sockfd))
{
    _pChannel->setReadCallback(
      bind(&TcpConnection::handleRead, this));
  // 通道可写事件到来的时候，回调TcpConnection::handleWrite
    _pChannel->setWriteCallback(
      bind(&TcpConnection::handleWrite, this));
  // 连接关闭，回调TcpConnection::handleClose
    //不能直接在这里调用enableRreading（）因为所属的线程可能在阻塞
    //必须通过runInLoop异步调用
}

TcpConnection::~TcpConnection()
{}

void TcpConnection::handleRead()
{
    int sockfd = _pChannel->getfd();
    int readlength;
    char line[MAX_LINE];
    if(sockfd < 0)
    {
        cout << "EPOLLIN sockfd < 0 error " << endl;
        return;
    }
    bzero(line, MAX_LINE);
    if((readlength = read(sockfd, line, MAX_LINE)) < 0)
    {
        if(errno == ECONNRESET)
        {
            cout << "ECONNREST closed socket fd:" << sockfd << endl;
            close(sockfd);
        }
    }
    else if(readlength == 0)
    {
        cout << "read 0 closed socket fd:" << sockfd << endl;
        close(sockfd);
    }
    else
    {
        string linestr(line, readlength);
        _inBuf.append(linestr);
        _messageCallback(shared_from_this(), &_inBuf);
    }
}

void TcpConnection::handleWrite()
{
    int sockfd = _pChannel->getfd();
    if(_pChannel->isWriting())
    {
        int n = ::write(sockfd, _outBuf.peek(), _outBuf.readableBytes());
        if( n > 0)
        {
            cout << "write " << n << " bytes data again" << endl;
            _outBuf.retrieve(n);
            if(_outBuf.readableBytes() == 0)
            {
                _pChannel->disableWriting(); //remove EPOLLOUT
                if (_writeCompleteCallback)		// 回调writeCompleteCallback_
                {
                    // 应用层发送缓冲区被清空，就回调用writeCompleteCallback_
                    _ploop->queueInLoop(boost::bind(_writeCompleteCallback, shared_from_this()));
                }
            }
        }
    }
}

//线程安全，可以跨线程调用
void TcpConnection::send(const string& message)
{
    if(_ploop->isInLoopThread())
    {
        sendInLoop(message);
    }
    else
    {
        _ploop->runInLoop(boost::bind(&TcpConnection::sendInLoop,this,message));
    }
}



void TcpConnection::sendInLoop(const string& message)
{
    int n = 0;
    //发送缓冲区没有数据，直接write
    if(_outBuf.readableBytes() == 0)
    {
        n = ::write(_sockfd, message.c_str(), message.size());
        if(n < 0)
            cout << "write error" << endl;
        if(n == static_cast<int>(message.size())&&_writeCompleteCallback)
        {
            _ploop->queueInLoop(boost::bind(_writeCompleteCallback, shared_from_this()));
        }
    }

    //发送缓冲区有数据，说明内核发送缓冲区满，要将所有的数据添加到output buffer中
    if( n < static_cast<int>(message.size()))
    {
        _outBuf.append(message.substr(n, message.size()));
        if(!_pChannel->isWriting())
        {
            _pChannel->enableWriting(); //add EPOLLOUT
        }
    }
}

void TcpConnection::connectEstablished()
{
  assert(_ploop->isInLoopThread());
  _pChannel->enableReading();	// TcpConnection所对应的通道加入到Poller关注
  _connectionCallback(shared_from_this());
}

