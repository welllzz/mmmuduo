//author voidccc
#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <vector>
#include <assert.h>

using namespace std;

class Buffer
{
    public:
        static const size_t Ksize = 1024;
        static const size_t Kprepend = 8;
        Buffer():_buffer(Ksize+Kprepend),_readIndex(Kprepend),_writeIndex(Kprepend){};

        size_t readableBytes() const {return _writeIndex-_readIndex;}
        size_t writeableBytes() const {return _buffer.size()-_writeIndex;}
        
        //开始读的位置的指针
        char* peek() {return &(*_buffer.begin())+_readIndex;}
        //写位置对应的指针
        char* beginWrite() {return &(*_buffer.begin())+_writeIndex;}
        
        //从套接字中读取数据
        ssize_t readFd(int fd,int* savedErrno);

        //写数据
        void append(const string& msg)
        {
            append(msg.c_str(),msg.size());
        }
        //写数据
        void append(const char* msg,size_t len)
        {
            if(len>writeableBytes()) makeSpace(len);
            std::copy(msg,msg+len,beginWrite());
            _writeIndex+=len;
        }
        

        //让用户读走len个字节数据
        string retrieve(size_t len)
        {
            assert(len<=readableBytes());
            string res(peek(),len);
            _readIndex+=len;
            if(_readIndex==_writeIndex)
            {
                //读走全部数据，重置
                _readIndex=Kprepend;
                _writeIndex=Kprepend;
            }
            return res;
        }

        //让用户读取全部数据
        string retrieveAll()
        {
            return retrieve(readableBytes());
        }

    private:
        void makeSpace(int len)
        {
            if(writeableBytes()+_readIndex<len+Kprepend)
            {
                _buffer.resize(_writeIndex+len);
            }
            else
            {
                //将缓冲区剩余的数据移到钱前面
                size_t readable = readableBytes();
                std::copy(&(*_buffer.begin())+_readIndex,
                            &(*_buffer.begin())+_writeIndex,
                                &(*_buffer.begin())+Kprepend);
                _readIndex = Kprepend;
                _writeIndex = Kprepend+readable;
            }
        }

        std::vector<char> _buffer;
        size_t _readIndex;
        size_t _writeIndex;
};

#endif
