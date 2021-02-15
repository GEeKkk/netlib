#include "Buffer.h"

#include <sys/uio.h>
#include <errno.h>
// #include "base/Common.h"
#include "netlib/base/Logging.h"

size_t Buffer::Readable() const {
    return m_writeIndex - m_readIndex;
}

size_t Buffer::Writeable() const {
    return m_buffer.size() - m_writeIndex;
}

size_t Buffer::Prependable() const {
    return m_readIndex;
}

char* Buffer::begin() {
    return m_buffer.data();
}

const char* Buffer::begin() const {
    return m_buffer.data();
}

void Buffer::EnsureWritable(size_t len) {
    if (Writeable() < len) {
        AddExtraSpace(len);
    }
}

void Buffer::Append(const char* data, size_t len) {
    EnsureWritable(len);
    std::copy(data, data + len, begin() + m_writeIndex);
    m_writeIndex += len;
}


void Buffer::AddExtraSpace(size_t len) {
    if (Writeable() + Prependable() < len  + kInitPrepend) {
        m_buffer.resize(m_writeIndex + len);
    } else {
        size_t dataLen = Readable();
        std::copy(begin() + m_readIndex, begin() + m_writeIndex, begin() + kInitPrepend);
        m_readIndex = kInitPrepend;
        m_writeIndex = m_readIndex + dataLen;
    }
}

ssize_t Buffer::ReadDataFromFd(int fd, int* savedErrNo) {
    char extraBuf[65536];
    struct iovec tmpVec[2];
    const size_t nWritable = Writeable();

    tmpVec[0].iov_base = begin() + m_writeIndex;
    tmpVec[0].iov_len = nWritable;

    tmpVec[1].iov_base = extraBuf;
    tmpVec[1].iov_len = sizeof(extraBuf);

    const ssize_t nRead = readv(fd, tmpVec, 2);
    if (nRead < 0) {
        *savedErrNo = errno;
    } else if (implicit_cast<size_t>(nRead) <= nWritable) {
        m_writeIndex += nRead;
    } else {
        m_writeIndex = m_buffer.size();
        Append(extraBuf, nRead - nWritable);
    }

    return nRead;
}