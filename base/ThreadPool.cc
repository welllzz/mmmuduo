
#include "ThreadPool.h"

#include <boost/bind.hpp>
#include <assert.h>
#include <stdio.h>

ThreadPool::ThreadPool(const string& name)
    :_mutex(),
    _cond(_mutex),
    _name(name),
    _running(false)
    {}

ThreadPool::~ThreadPool()
{
    if(_running)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(_threads.empty());
    _running = true;
    _threads.reserve(numThreads);
    for(int i=0;i<numThreads;++i)
    {
        char id[32];
        snprintf(id,sizeof(id),"%d",i);
        cout << "pthread " << i << " haved been created" << endl;
        _threads.push_back(new Thread(boost::bind(&ThreadPool::runInThread,this),_name+id));
        _threads[i].start();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(_mutex);
        _running = false;
        _cond.notifyAll();
    }
    for_each(_threads.begin(),_threads.end(),boost::bind(&Thread::join,_1));
}

void ThreadPool::run(const Task& task)
{
    if(_threads.empty()) task();
    else
    {
        MutexLockGuard lock(_mutex);
        _queue.push_back(task);
        _cond.notify();
    }
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(_mutex);
    while(_queue.empty()&&_running)
        _cond.wait();
    Task task;
    if(!_queue.empty())
    {
        task = _queue.front();
        _queue.pop_front();
    }
    return task;
}

void ThreadPool::runInThread()
{
    while(_running)
    {
        Task task(take());
        if(task) task();
    }
}

 