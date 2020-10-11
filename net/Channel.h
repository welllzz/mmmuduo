#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
class EventLoop;

class Channel
{
    public:

        typedef std::function<void()> EventCallback;
        Channel(EventLoop* pLoop, int sockfd);
        ~Channel();
        
        void handleEvent();

        void setReadCallback(const EventCallback& cb)
        { _readCallback = cb; }
        void setWriteCallback(const EventCallback& cb)
        { _writeCallback = cb; }
        void setCloseCallback(const EventCallback& cb)
        { _closeCallback = cb; }
        void setErrorCallback(const EventCallback& cb)
        { _errorCallback = cb; }

        void setRevents(int revents){_revents=revents;}
        void setIndex(int index){_index=index;};
        void enableReading();
        void enableWriting();
        void disableWriting();
        bool isWriting();
        
        int getEvents(){return _events;};
        int getfd(){return _sockfd;};
        int getIndex(){return _index;};
    private:
        void update();
        
        int _sockfd;
        int _events;
        int _revents;
        int _index;//表示状态
        EventLoop* _pLoop;

        EventCallback _readCallback;
        EventCallback _writeCallback;
        EventCallback _closeCallback;
        EventCallback _errorCallback;
        
};
#endif