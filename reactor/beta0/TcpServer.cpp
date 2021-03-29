#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"

#include "IOThreadPool.h"

using namespace std;
using namespace muduo;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : m_AcceptorLoop(loop),
      m_acceptor(make_unique<Acceptor>(loop, listenAddr)),
      m_IoThreadPool(make_unique<IOThreadPool>(loop)),
      m_started(false),
      m_nextConnId(1),
      m_name(listenAddr.toHostPort())
{
    m_acceptor->SetConnCallback(bind(&TcpServer::HandleNewConn, this, placeholders::_1, placeholders::_2));
}

TcpServer::~TcpServer() {

}

void TcpServer::Start() {
    if (!m_started) {
        m_started = true;
        m_IoThreadPool->Start();
    }
    if (!m_acceptor->IsListenning()) {
        m_AcceptorLoop->RunInLoop(bind(&Acceptor::Listen, m_acceptor.get()));
    }
}

void TcpServer::SetThreadNum(int num) {
    m_IoThreadPool->SetThreadNum(num);
}

// 非线程安全函数，需要保证在IO线程操作
void TcpServer::HandleNewConn(int connfd, const InetAddress& peerAddr) {
    m_AcceptorLoop->CheckInLoopThread();
    // 设置Tcp连接名称
    string tmpName("#");
    tmpName.append(to_string(m_nextConnId));
    string connName = m_name + tmpName;
    ++m_nextConnId;
    InetAddress localAddr(sockets::getLocalAddr(connfd));
    // 从线程池中拿出一个IO线程处理连接
    EventLoop* ioLoop = m_IoThreadPool->GetNextLoop();
    // 创建Tcp连接对象
    TcpConnPtr conn = make_shared<TcpConn>(ioLoop, connName, connfd, localAddr, peerAddr);
    // 连接集合
    m_connMap[connName] = conn;
    // 连接建立
    conn->SetConnCallback(m_connCallback);
    // 消息到来
    conn->SetMsgCallback(m_msgCallback);
    // 关闭事件
    conn->SetCloseCallback(bind(&TcpServer::RemoveConn, this, placeholders::_1));
    // 发送缓冲区为空，继续发送事件
    conn->SetWriteCompleteCallback(m_writeCompleteCallback);

    ioLoop->RunInLoop(bind(&TcpConn::ConnEstablished, conn));
}

void TcpServer::RemoveConnInLoop(const TcpConnPtr& conn) {
    m_AcceptorLoop->CheckInLoopThread();
    m_connMap.erase(conn->name());
    EventLoop* ioLoop = conn->GetLoop();
    ioLoop->Stored(bind(&TcpConn::ConnDestroyed, conn));
}

void TcpServer::RemoveConn(const TcpConnPtr& conn) {
    m_AcceptorLoop->RunInLoop(bind(&TcpServer::RemoveConnInLoop, this, conn));
}