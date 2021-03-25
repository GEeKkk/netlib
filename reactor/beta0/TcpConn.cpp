#include "TcpConn.h"
#include "Channel.h"
#include "EventLoop.h"
#include "SocketsOps.h"

using namespace muduo;
using namespace std;

TcpConn::TcpConn(EventLoop* loop,
                 const string& name,
                 int sockfd,
                 const InetAddress& local,
                 const InetAddress& peer)
    : m_loop(loop),
      m_name(name),
      m_state(kConnecting),
      m_socket(make_unique<Socket>(sockfd)),
      m_channel(make_unique<Channel>(loop, sockfd)),
      m_localAddr(local),
      m_peerAddr(peer)
{
    m_channel->SetRead(std::bind(&TcpConn::HandleRead, this));
}

TcpConn::~TcpConn() {

}

bool TcpConn::Connected() const {
    return m_state == kConnected;
}

void TcpConn::ConnEstablished() {
    m_loop->CheckInLoopThread();
    SetState(kConnected);
    m_channel->EnableRead();
    m_connCallback(shared_from_this());
}

void TcpConn::ConnDestroyed() {
    m_loop->CheckInLoopThread();
    SetState(kDisconnected);
    m_channel->DisableAll();
    m_connCallback(shared_from_this());
    m_loop->RemoveChannel(m_channel.get());
}

void TcpConn::SetConnCallback(const TcpConnCallback& cb) {
    m_connCallback = cb;
}

void TcpConn::SetMsgCallback(const TcpMsgCallback& cb) {
    m_msgCallback = cb;
}

void TcpConn::SetCloseCallback(const TcpCloseCallback& cb) {
    m_closeCallback = cb;
}

void TcpConn::SetState(ConnState st) {
    m_state = st;
}

void TcpConn::HandleRead() {
    char buf[65536];
    ssize_t len = read(m_channel->fd(), buf, sizeof(buf));
    if (len > 0) {
        m_msgCallback(shared_from_this(), buf, len);
    } else if (len == 0) {
        HandleClose();
    } else {
        HandleError();
    }
}

void TcpConn::HandleWrite() {

}

void TcpConn::HandleClose() {
    m_channel->DisableAll();
    m_closeCallback(shared_from_this());
}

void TcpConn::HandleError() {
    
}