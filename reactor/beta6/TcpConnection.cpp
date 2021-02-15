#include "TcpConnection.h"

#include "netlib/base/Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "NetAddr.h"
#include "SocketUtil.h"

#include <errno.h>
#include <stdio.h>

using namespace muduo;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const NetAddr& localAddr,
                             const NetAddr& peerAddr)
    : m_loop(loop),
      m_name(name),
      m_state(kConnecting),
      m_pSocket(std::make_unique<Socket>(sockfd)),
      m_pChan(std::make_unique<Channel>(loop, sockfd)),
      m_localAddr(localAddr),
      m_peerAddr(peerAddr)
{
    LOG_DEBUG << "TcpConnection::ctor[" << m_name << "] at " << this
              << " fd = " << sockfd;
    
    m_pChan->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG << "TcpConnection::dtor[" << m_name << "] at " << this
              << " fd = " << m_pChan->GetFd();
}


void TcpConnection::ConnectEstablished() {
    m_loop->CheckInLoopThread();

    SetTcpState(kConnected);
    m_pChan->EnableRead();
    m_connHandler(shared_from_this());
}

void TcpConnection::ConnectDestroyed() {
    m_loop->CheckInLoopThread();

    SetTcpState(kDisconnected);
    m_pChan->DisableAll();
    m_connHandler(shared_from_this());

    m_loop->RemoveChannel(m_pChan.get());
}

void TcpConnection::HandleRead() {
    char buf[66535];
    auto n = ::read(m_pChan->GetFd(), buf, sizeof(buf));
    if (n > 0) {
        m_msgHandler(shared_from_this(), buf, n);
    } else if (n == 0) {
        HandleClose();
    } else {
        HandleError();
    }
}

void TcpConnection::HandleError() {
    int err = SocketUtil::getSocketError(m_pChan->GetFd());
    LOG_ERROR << "TcpConnection::HandleError [" << m_name 
              << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}


void TcpConnection::HandleClose() {
    m_loop->CheckInLoopThread();
    LOG_DEBUG << "TcpConnection::HandleClose state = " << m_state;
    m_pChan->DisableAll();
    m_closeHandler(shared_from_this());
}

EventLoop* TcpConnection::GetLoop() const {
    return m_loop;
}

const std::string& TcpConnection::GetName() const {
    return m_name;
}

const NetAddr& TcpConnection::GetLocalAddr() {
    return m_localAddr;
}

const NetAddr& TcpConnection::GetPeerAddr() {
    return m_peerAddr;
}

bool TcpConnection::IsConnected() const {
    return m_state == kConnected;
}

void TcpConnection::SetConnectionHandler(const ConnectionHandler& hd) {
    m_connHandler = hd;
}

void TcpConnection::SetMessageHandler(const MessageHandler& hd) {
    m_msgHandler = hd;
}

void TcpConnection::SetCloseHandler(const CloseHandler& hd) {
    m_closeHandler = hd;
}

void TcpConnection::SetTcpState(TcpState ts) {
    m_state = ts;
} 