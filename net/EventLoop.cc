//author voidccc

#include <sys/eventfd.h>

#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "CurrentThread.h"

#include <iostream>
using namespace std;


int EventLoop::createEventfd()
{
   int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
   if (evtfd < 0)
   {
       cout << "Failed in eventfd" << endl;
   }
   return evtfd;
}

EventLoop::EventLoop()
    :_looping(false)
    ,_quit(false)
    ,_callingPendingFunctors(false)
    ,_eventfd(createEventfd())
    ,_threadId(CurrentThread::tid())
    ,_pPoller(new Epoll()) // Memory Leak !!!//用智能指针
     // Memory Leak!!!
    ,_pEventfdChannel(new Channel(this, _eventfd))
    ,_mutex()
{
    _pEventfdChannel->setReadCallback(bind(&EventLoop::handleRead,this));
    _pEventfdChannel->enableReading();
}

EventLoop::~EventLoop()
{
    ::close(_eventfd);
}


void EventLoop::loop()
{
    assert(!_looping);
    _looping = true;
    _quit = false;
    while(!_quit)
    {
        vector<Channel*> activeChannels;//传出参数，返回结果
        _pPoller->poll(&activeChannels);

        vector<Channel*>::iterator it;
        for(it = activeChannels.begin(); it != activeChannels.end(); ++it)
        {
            (*it)->handleEvent();
        }

        doPendingFunctors();
    }
    _looping = false;
}

//可以跨线程调用
void EventLoop::quit()
{
    _quit = true;
    if(!isInLoopThread()) wakeup();
}

void EventLoop::update(Channel* pChannel)
{
    _pPoller->update(pChannel);
}

void EventLoop::runInLoop(const Functor& task)
{
    if(isInLoopThread())//如果是在本线程
    {
        task();
    }
    else
    {
        queueInLoop(task);
    }
}

void EventLoop::queueInLoop(const Functor& task)
{
    {
        //这里需要加锁，因为可能多个线程同时访问
        MutexLockGuard guard(_mutex);
        _pendingFunctors.push_back(task);
    }

    if(!isInLoopThread() || _callingPendingFunctors)
    {
        wakeup();
    }
}


//向_evnentfd中写入一个字节数据唤醒epoll
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(_eventfd, &one, sizeof one);
    if (n != sizeof one)
    {
        cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << endl;
    }
}

//读走数据，不然会一直触发
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(_eventfd, &one, sizeof one);
    if (n != sizeof one)
    {
        cout << "EventEventLoop::handleRead() reads " << n << " bytes instead of 8" << endl;
    }
}


void EventLoop::doPendingFunctors()
{
    vector<Functor> tempRuns;
    _callingPendingFunctors = true;
    {
        MutexLockGuard guard(_mutex);
        tempRuns.swap(_pendingFunctors);
    }
    for(auto it = tempRuns.begin(); it != tempRuns.end(); ++it)
    {
        (*it)();
    }
    _callingPendingFunctors = false;
}



bool EventLoop::isInLoopThread()
{
    return _threadId == CurrentThread::tid();
}
