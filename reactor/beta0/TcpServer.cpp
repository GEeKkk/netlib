#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"

using namespace std;
using namespace muduo;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : m_loop(loop),
      m_acceptor(make_unique<Acceptor>(loop, listenAddr)),
      m_started(false),
      m_nextConnId(1),
      m_name(listenAddr.toHostPort())
{
    m_acceptor->SetConnHandler(std::bind(&TcpServer::HandleOneConn, this, placeholders::_1, placeholders::_2));
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
void TcpServer::HandleOneConn(int connfd, const InetAddress& peerAddr) {
    m_loop->CheckInLoopThread();
    // 设置Tcp连接名称
    string tmpName("#");
    tmpName.append(to_string(m_nextConnId));
    string connName = m_name + tmpName;
    ++m_nextConnId;
    InetAddress localAddr(sockets::getLocalAddr(connfd));
    // 创建Tcp连接对象
    TcpConnPtr conn(make_shared<TcpConn>(m_loop, connName, connfd, localAddr, peerAddr));
    m_connMap[connName] = conn;
    // 用户读写事件回调
    conn->SetConnCallback(m_connCallback);
    conn->SetMsgCallback(m_msgCallback);
    // 库内关闭事件回调
    conn->SetCloseCallback(bind(&TcpServer::RemoveOneConn, this, placeholders::_1));
    // 发送缓冲区为空，继续发送事件
    conn->SetWriteCompleteCallback(m_writeCompleteCallback);
    conn->ConnEstablished();
}

void TcpServer::RemoveOneConn(const TcpConnPtr& conn) {
    m_loop->CheckInLoopThread();
    m_connMap.erase(conn->name());
    m_loop->Stored(bind(&TcpConn::ConnDestroyed, conn));
}