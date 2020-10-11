//author voidccc
#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Declear.h"
#include <boost/function.hpp>
#include "Buffer.h"
#include <memory>
#include <functional>
#include <string>

#include <boost/enable_shared_from_this.hpp>

using namespace std;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>

{
    public:
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef boost::function<void (const TcpConnectionPtr&,Buffer*)> MessageCallback;
    typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
    typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

        TcpConnection(int sockfd, EventLoop* pLoop);
        ~TcpConnection();

        void send(const string& message);
        void sendInLoop(const string& message);
        void connectEstablished();

        void setConnectionCallback(const ConnectionCallback& cb)
        { _connectionCallback = cb; }

        void setMessageCallback(const MessageCallback& cb)
        { _messageCallback = cb; }

        void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        { _writeCompleteCallback = cb; }

    private:
        void handleRead();
        void handleWrite();

        int _sockfd;
        EventLoop* _ploop;
        std::unique_ptr<Channel> _pChannel;
        Buffer _inBuf;			// 应用层接收缓冲区
        Buffer _outBuf;			// 应用层发送缓冲区   

        ConnectionCallback _connectionCallback;
        MessageCallback _messageCallback;
        WriteCompleteCallback _writeCompleteCallback;
};

#endif
