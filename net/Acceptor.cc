//author voidccc

#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"

#include <iostream>
using namespace std;

int creatFd()
{
    int on = 1;
    int listenfd = ::socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    ::fcntl(listenfd, F_SETFL, O_NONBLOCK); //no-block io
    ::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    servaddr.sin_port = ::htons(11111);

    if(-1 == ::bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        cout << "bind error, errno:" << errno << endl;
    }
    return listenfd;
}

Acceptor::Acceptor(EventLoop* pLoop)
    :_listenfd(creatFd())
    ,_listenning(false)
    ,_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
    ,_pLoop(pLoop)
    ,_acceptChannel(_pLoop,_listenfd)
{}

Acceptor::~Acceptor()
{}

void Acceptor::listen()
{
    _listenning = true;
    if(-1 == ::listen(_listenfd, MAX_LISTENFD))
    {
        cout << "listen error, errno:" << errno << endl;
    }
    _acceptChannel.setReadCallback(bind(&Acceptor::handleRead,this));
    _acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(struct sockaddr_in);
    connfd = ::accept(_listenfd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
    if(connfd > 0)
    {
        cout << "new connection from "
            << "[" << inet_ntoa(cliaddr.sin_addr) 
            << ":" << ntohs(cliaddr.sin_port) << "]"
            << " new socket fd:" << connfd
            << endl;
        ::fcntl(connfd, F_SETFL, O_NONBLOCK); //no-block io

        if(_newConntion) _newConntion(connfd);
        else ::close(connfd);
    }
    else
    {
        cout << "accept error, connfd:" << connfd
            << " errno:" << errno << endl;
        if(errno==EMFILE)//当文件描述符不足
        {
            ::close(_idleFd);
            _idleFd = ::accept(_listenfd, NULL, NULL);
            ::close(_idleFd);
            _idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }

}


