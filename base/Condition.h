#ifndef CONDITION_H
#define CONDITION_H

#include "Mutex.h"
#include <pthread.h>

class Condition:boost::noncopyable
{
    public:
        explicit Condition(MutexLock& mutex)
        :_mutex(mutex)
        {
            pthread_cond_init(&_pcond,NULL);
        }

        ~Condition()
        {
            pthread_cond_destroy(&_pcond);
        }

        void wait()
        {
            pthread_cond_wait(&_pcond,_mutex.getPthreadMutex());
        }

        void notify()
        {
            pthread_cond_signal(&_pcond);
        }

        void notifyAll()
        {
            pthread_cond_broadcast(&_pcond);
        }
    
    private:
        MutexLock& _mutex;
        pthread_cond_t _pcond;
};

#endif