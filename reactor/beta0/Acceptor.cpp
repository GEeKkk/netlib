#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

using namespace muduo;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr)
    : m_loop(loop),
      m_AcceptSocket(sockets::createNonblockingOrDie()),
      m_AcceptChannel(loop, m_AcceptSocket.fd()),
      m_listenning(false)
{
    m_AcceptSocket.setReuseAddr(true);
    m_AcceptSocket.bindAddress(addr);
    m_AcceptChannel.SetRead(std::bind(&Acceptor::HandleRead, this));
}

void Acceptor::Listen() {
    m_loop->CheckInLoopThread();
    m_listenning = true;
    m_AcceptSocket.listen();
    m_AcceptChannel.EnableRead();
}

bool Acceptor::IsListenning() const {
    return m_listenning;
}

void Acceptor::SetConnHandler(const ConnHandler& hd) {
    m_ConnHandler = hd;
}

void Acceptor::HandleRead() {
    m_loop->CheckInLoopThread();
    InetAddress peerAddr(0);
    int connfd = m_AcceptSocket.accept(&peerAddr);
    if (connfd >= 0) {
        if (m_ConnHandler) {
            m_ConnHandler(connfd, peerAddr);
        } else {
            printf("No ConnHnadler, close\n");
            close(connfd);
        }
    } 
}