#include "Socket.h"

#include "NetAddr.h"
#include "SocketUtil.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>


Socket::~Socket() {
    SocketUtil::close(m_sockfd);
}

void Socket::Bind(const NetAddr& localAddr) {
    SocketUtil::bindOrDie(m_sockfd, localAddr.GetSockAddr());
}

void Socket::Listen() {
    SocketUtil::listenOrDie(m_sockfd);
}

int Socket::Accept(NetAddr* peerAddr) {
    struct sockaddr_in addr;
    int connfd = SocketUtil::accept(m_sockfd, &addr);
    if (connfd >= 0) {
        peerAddr->SetSockAddr(addr);
    }
    return connfd;
}

void Socket::SetReuseAddr(bool on) {
    int opt = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR,
                 &opt, sizeof(opt));
}

int Socket::GetSockFd() const {
    return m_sockfd;
}

void Socket::ShutdownWrite() {
    SocketUtil::shutdownWrite(m_sockfd);
}