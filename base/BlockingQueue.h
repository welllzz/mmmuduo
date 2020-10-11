#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H


#include "Condition.h"
#include "Mutex.h"
#include <boost/noncopyable.hpp>
#include <deque>
#include <assert.h>

template<typename T>
class BlockingQueue : boost::noncopyable
{
    public:
        BlockingQueue()
            :_mutex(),
             _notEmpty(_mutex),
             _queue()
        {}

        ~BlockingQueue(){}

        void put(const T& x)
        {
            MutexLockGuard lock(_mutex);
            _queue.push_back(x);
            _notEmpty.notify();
        }

        T take()
        {
            MutexLockGuard lock(_mutex);
            while(_queue.empty())
                _notEmpty.wait();
            assert(!_queue.emtpy());
            T front(_queue.front());
            _queue.pop_back();
            return front;
        }

         size_t size() const
        {
            MutexLockGuard lock(_mutex );
            return _queue.size();
        }

    private:
        mutable MutexLock _mutex;
        Condition _notEmpty;
        std::deque<T> _queue;
};

#endif