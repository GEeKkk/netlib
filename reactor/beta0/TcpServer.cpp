#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"

using namespace std;
using namespace muduo;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : m_loop(loop),
      m_acceptor(make_unique<Acceptor>(loop, listenAddr)),
      m_started(false),
      m_nextConnId(1),
      m_name(listenAddr.toHostPort())
{
    m_acceptor->SetConnHandler(std::bind(&TcpServer::HandleOneConn, this, _1, _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::Start() {
    if (!m_started) {
        m_started = true;
    }
    if (!m_acceptor->IsListenning()) {
        m_loop->RunInLoop(bind(&Acceptor::Listen, m_acceptor.get()));
    }
}

// 非线程安全函数，需要保证在IO线程操作
void TcpServer::HandleOneConn(int sockfd, const InetAddress& peerAddr) {
    m_loop->CheckInLoopThread();
    string tmpName("#");
    tmpName.append(to_string(m_nextConnId));
    string connName = m_name + tmpName;
    ++m_nextConnId;
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnPtr conn(make_shared<TcpConn>(m_loop, connName, sockfd, localAddr, peerAddr));
    m_connMap[connName] = conn;
    conn->SetConnCallback(m_connCallback);
    conn->SetMsgCallback(m_msgCallback);
    conn->ConnEstablished();
}
void TcpServer::SetConnCallback(const TcpConnCallback& cb) {
    m_connCallback = cb;
}
void TcpServer::SetMsgCallback(const TcpMsgCallback& cb) {
    m_msgCallback = cb;
}