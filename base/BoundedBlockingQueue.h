#ifndef BOUNDEDBLOCKINGQUEUE_H
#define BOUNDEDBLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"
#include <boost/circular_buffer.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>

template<typename T>
class BoundedBlockingQueue : boost::noncopyable
{
    public:
        BoundedBlockingQueue(int maxSize)
            :_mutex(),
            _notFull(_mutex),
            _notEmpty(_mutex),
            _queue(maxSize)
        {
        }

        void put(const T& x)
        {
            MutexLockGuard lock(_mutex);
            while(_queue.full)//看书找答案 虚假唤醒
                _notFull.wait();
        
            assert(!_queue.full());
            _queue.push_back(x);
            _notEmpty.notify();
        }

        T take()
        {
            MutexLockGuard lock(_mutex);
            while(_queue.empty())
                _notEmpty.wait();
            assert(!_queue.empty());
            _notFull.notify();
            auto front=_queue.front();
            _queue.pop_front();
            return front;
        }

        bool empty() const
        {
            MutexLockGuard lock(_mutex);
            return _queue.empty();
        }

        bool full() const
        {
            MutexLockGuard lock(_mutex);
            return _queue.full();
        }

        size_t size() const
        {
            MutexLockGuard lock(_mutex);
            return _queue.size();
        }

        size_t capacity() const
        {
            MutexLockGuard lock(_mutex);
            return _queue.capacity();
        }

    private:
        mutable MutexLock _mutex;
        Condition _notFull;
        Condition _notEmpty;
        boost::circular_buffer<T> _queue;//循环队列
};
#endif