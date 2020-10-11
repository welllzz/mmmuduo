#include "Thread.h"
#include <errno.h>


namespace CurrentThread
{
    // 外部连接
  __thread int t_cachedTid = 0;
}


void Thread::start()
{
    assert(!_started);
    _started = true;
    errno = pthread_create(&_pthreadId,NULL,&startThread,this);
    if(errno!=0) cout << "Failed in pthread_create" << endl;
}

Thread::~Thread(){}

int Thread::join()
{
    assert(_started);
    return pthread_join(_pthreadId,NULL);
}

void* Thread::startThread(void* obj)
{
    Thread* thread = static_cast<Thread*>(obj);
    thread->runInThread();
    return NULL;
}

void Thread::runInThread()
{
    assert(_started);
    //cout << "pthread " << _pthreadId << endl;
    _func();
}

