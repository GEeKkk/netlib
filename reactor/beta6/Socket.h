#ifndef SOCKET_H
#define SOCKET_H

#include "netlib/base/noncopyable.h"

class NetAddr;

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd) : m_sockfd(sockfd) {}
    ~Socket();

    void Bind(const NetAddr& localAddr);
    void Listen();
    int Accept(NetAddr* peerAddr);

    void SetReuseAddr(bool on);
    int GetSockFd() const;

private:
    const int m_sockfd;
};

#endif // SOCKET_H