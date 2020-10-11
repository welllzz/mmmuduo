#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>
#include <iostream>

Channel::Channel(EventLoop* pLoop, int sockfd)
    :_sockfd(sockfd)
    ,_events(0)
    ,_revents(0)
    ,_index(-1)
    ,_pLoop(pLoop)
{
}

void Channel::handleEvent()
{
   if(_revents & EPOLLIN)
   {
      _readCallback();
   }
   if(_revents & EPOLLOUT)
   {
      _writeCallback();
   }
}


void Channel::enableReading()
{
    _events |= EPOLLIN;
    update();
}

void Channel::enableWriting()
{
    _events |= EPOLLOUT;
    update();
}

void Channel::disableWriting()
{
    _events &= ~EPOLLOUT;
    update();
}

bool Channel::isWriting()
{
    return _events & EPOLLOUT;
}

void Channel::update()
{
    _pLoop->update(this);
}