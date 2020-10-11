//author voidccc
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Declear.h"
#include "Define.h"
#include <functional>
#include "Channel.h"

class Acceptor 
{
    public:
        typedef std::function<void(int sockfd)> NewConnectionCallback;
        
        Acceptor(EventLoop* pLoop);
        ~Acceptor();

        void listen();
        void setNewConnectionCallback(NewConnectionCallback cb)
            {_newConntion = cb;}
        bool listenning(){return _listenning;}
    private:
        void handleRead();
        int createAndListen();

        int _listenfd;
        bool _listenning;
        int _idleFd;
        EventLoop* _pLoop;
        Channel _acceptChannel;

        NewConnectionCallback _newConntion;
        
};

#endif
