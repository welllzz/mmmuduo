
#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H 

#include <unistd.h>
#include <sys/syscall.h>

//这里使用__thread 线程内部数据
//用于表示线程，不能使用pthread_self函数
namespace CurrentThread
{
    extern __thread int t_cachedTid;
    
    inline void cacheTid()
    {
        t_cachedTid = static_cast<int>(::syscall(SYS_gettid));
    }
    //避免多次系统调用，用一个缓存
    inline int tid()
    {
        if(t_cachedTid == 0)
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}

#endif
