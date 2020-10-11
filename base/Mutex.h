#ifndef MUTEX_H
#define MUTEX_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

class MutexLock:boost::noncopyable
{
    public:
        MutexLock()
        {
        }

        ~MutexLock()
        {
            int ret = pthread_mutex_destroy(&_mutex);
            assert(ret==0);(void)ret;
        }

        void lock()
        {
            pthread_mutex_lock(&_mutex);
        }

        void unlock(){pthread_mutex_unlock(&_mutex);}
        pthread_mutex_t* getPthreadMutex(){return &_mutex;}
        
    private:
        pthread_mutex_t _mutex;
    
};

class MutexLockGuard : boost::noncopyable
{
    public:
        MutexLockGuard(MutexLock& mutex)
            :_mutex(mutex)
        {
            _mutex.lock();
        }

        ~MutexLockGuard(){_mutex.unlock();}

    private:
        MutexLock& _mutex;
};

#define MutexLockGuard(x) error "Missing guard object name"

#endif