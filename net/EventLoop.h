//author voidccc
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Declear.h"
#include "base/Mutex.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <memory>
#include <vector>
using namespace std;

class EventLoop
{
    public:
        //执行异步任务
        typedef boost::function<void()> Functor;

        EventLoop();
        ~EventLoop();
        void loop();
        void quit();

        void update(Channel* pChannel);//更新epoll的关注时间
        void queueInLoop(const Functor& task);//异步调用
        void runInLoop(const Functor& task);

        bool isInLoopThread();//判断是否在本线程内
        
        void wakeup();//用来异步唤醒
        
    private:
        //解决_eventfd的读事件
        void handleRead();
        int createEventfd();
        void doPendingFunctors();

        bool _looping;
        bool _quit;
        bool _callingPendingFunctors;
        int _eventfd;//专门用来执行异步任务的唤醒fd
        const pid_t _threadId;//当前线程所属的ID

        unique_ptr<Epoll> _pPoller;//生命周期与Eventloop一致
        unique_ptr<Channel> _pEventfdChannel;//生命周期与Eventloop一致

        MutexLock _mutex;
        std::vector<Functor> _pendingFunctors;//要执行的异步任务集合
};

#endif
