#ifndef THREADPOOL_H
#define THREADPOOL_H


#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <deque>

class ThreadPool:boost::noncopyable
{
    typedef boost::function<void()> Task;
    public:
        explicit ThreadPool(const string& name=string());

        ~ThreadPool();

        void start(int numThreads);
        void stop();
        void run(const Task& t);
        
    private:
        void runInThread();
        Task take();

        MutexLock _mutex;
        Condition _cond;
        string _name;
        boost::ptr_vector<Thread> _threads;
        std::deque<Task> _queue;
        bool _running;
};

#endif