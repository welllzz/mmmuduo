#include "Buffer.h"
#include <sys/uio.h>

// 结合栈上的空间，避免内存使用过大，提高内存使用率
// 如果有5K个连接，每个连接就分配64K+64K的缓冲区的话，将占用640M内存，
// 而大多数时候，这些缓冲区的使用率很低
ssize_t Buffer::readFd(int fd,int* savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    size_t writeable = writeableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writeable;

    //第二快缓冲区
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    ssize_t n = ::readv(fd,vec,2);
    if(n<0)
    {
        *savedErrno = errno;
    }
    else if(static_cast<size_t>(n)<=writeable)
    {
        _writeIndex+=n;
    }else
    {
        _writeIndex = _buffer.size();
        append(extrabuf,n-writeable);
    }
};