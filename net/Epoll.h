#ifndef EPOLL_H
#define EPOLL_H

#include <boost/noncopyable.hpp>
#include <vector>
#include <map>
#include <sys/epoll.h>

#include "Declear.h"
#include "Define.h"

class Epoll
{
public:
    Epoll();
    ~Epoll();
    void poll(std::vector<Channel*>* pChannels);
    void update(Channel* pChannel);
private:
    int _epollfd;
    struct epoll_event _events[MAX_EVENTS];
};

#endif