#include "Acceptor.h"

#include "netlib/base/Logging.h"
#include "EventLoop.h"
#include "NetAddr.h"
#include "SocketUtil.h"

using namespace muduo;

Acceptor::Acceptor(EventLoop* loop, const NetAddr& listenAddr)
    : m_loop(loop),
      m_AcceptSock(SocketUtil::createNonblockingOrDie()),
      m_AcceptChannel(loop, m_AcceptSock.GetSockFd()),
      m_listening(false)
{
    m_AcceptSock.SetReuseAddr(true);
    m_AcceptSock.Bind(listenAddr);
    m_AcceptChannel.SetReadCallback(std::bind(&Acceptor::HandleRead, this));
}

void Acceptor::Listen() {
    m_loop->CheckInLoopThread();
    m_listening = true;
    m_AcceptSock.Listen();
    m_AcceptChannel.EnableRead();
}

void Acceptor::HandleRead() {
    m_loop->CheckInLoopThread();
    NetAddr peerAddr(0);
    int connfd = m_AcceptSock.Accept(&peerAddr);
    if (connfd >= 0) {
        if (m_newConnHandler) {
            m_newConnHandler(connfd, peerAddr);
        } else {
            SocketUtil::close(connfd);
        }
    }
}

void Acceptor::SetNewConnectionHandler(const NewConnectionHandler& cb) {
    m_newConnHandler = cb;
}


bool Acceptor::IsListening() const {
    return m_listening;
}