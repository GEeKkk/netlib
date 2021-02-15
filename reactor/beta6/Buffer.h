#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <sys/types.h>

class Buffer
{
public:
    static const size_t kInitPrepend = 8;
    static const size_t kInitSize = 1024;
public:
    Buffer()
        : m_buffer(kInitPrepend + kInitSize),
          m_readIndex(kInitPrepend),
          m_writeIndex(kInitPrepend)
    {
    }

    size_t Readable() const;
    size_t Writeable() const;
    size_t Prependable() const;

    void EnsureWritable(size_t len);
    void Append(const char* data, size_t len);

    ssize_t ReadDataFromFd(int fd, int* savedErrno);

private:
    char* begin();
    const char* begin() const;

    void AddExtraSpace(size_t len);

private:
    std::vector<char> m_buffer;
    size_t m_readIndex;
    size_t m_writeIndex;
};

#endif // BUFFER_H