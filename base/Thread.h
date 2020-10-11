#ifndef THREAD_H
#define THREAD_H

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <pthread.h>
#include <iostream>

using namespace std;


class Thread : boost::noncopyable
{
    public:
        typedef boost::function<void()> ThreadFunc;

        explicit Thread(const ThreadFunc& func,const string& name=string())
        :_started(false),_func(func),_pthreadId(0),_name(name){};

        ~Thread();

        void start();
        int join();

        bool started() const {return _started;}
        const string& name() const{return _name;}

    private:
        static void* startThread(void* thread);//线程函数必须绑定静态的
        void runInThread();


        bool _started;
        ThreadFunc _func;
        pthread_t _pthreadId;
        string _name;
};
    

#endif