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
using namespace std::placeholders;

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
    
    m_pChan->SetReadCallback(std::bind(&TcpConnection::HandleRead, this, _1));
    m_pChan->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
    m_pChan->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
    m_pChan->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));

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

void TcpConnection::HandleRead(muduo::Timestamp recvTime) {
    int savedErrno = 0;
    ssize_t n = m_inputBuffer.readFd(m_pChan->GetFd(), &savedErrno);
    if (n > 0) {
        m_msgHandler(shared_from_this(), &m_inputBuffer, recvTime);
    } else if (n == 0) {
        HandleClose();
    } else {
        errno = savedErrno;
        LOG_SYSERR << "TcpConnection::HandleRead";
        HandleError();
    }
}

void TcpConnection::HandleWrite() {
    m_loop->CheckInLoopThread();
    if (m_pChan->IsWriting()) {
        ssize_t n = ::write(m_pChan->GetFd(), m_ouputBuffer.peek(), m_ouputBuffer.readableBytes());
        if (n > 0) {
            m_ouputBuffer.retrieve(n);
            if (m_ouputBuffer.readableBytes() == 0) {
                m_pChan->DisableWrite();
                if (m_state == kDisconnecting) {
                    ShutdownInLoop();
                }
            } else {
                LOG_DEBUG << "HandleWrite more data.";
            }
        } else {
            LOG_ERROR << "TcpConnection::HandleWrite";
        }
    } else {
        LOG_DEBUG << "Connection is down, no more writing";
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

void TcpConnection::Send(const std::string& message) {
    if (m_state == kConnected) {
        if (m_loop->IsInLoopThread()) {
            SendInLoop(message);
        } else {
            m_loop->RunInLoop(std::bind(&TcpConnection::SendInLoop, this, message));
        }
    }
}

void TcpConnection::SendInLoop(const std::string& message) {
    m_loop->CheckInLoopThread();
    ssize_t nwrote = 0;
    if (!m_pChan->IsWriting() && m_ouputBuffer.readableBytes() == 0) {
        nwrote = ::write(m_pChan->GetFd(), message.data(), message.size());
        if (nwrote > 0) {
            if (implicit_cast<size_t>(nwrote) < message.size()) {
                LOG_DEBUG << "nwrote < message.size()";
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << "TcpConnection::SendInLoop";
            }
        }
    }

    if (implicit_cast<size_t>(nwrote) < message.size()) {
        m_ouputBuffer.append(message.data() + nwrote, message.size() - nwrote);
        if (!m_pChan->IsWriting()) {
            m_pChan->EnableRead();
        }
    }
}

void TcpConnection::Shutdown() {
    if (m_state == kConnected) {
        SetTcpState(kDisconnecting);
        m_loop->RunInLoop(std::bind(&TcpConnection::ShutdownInLoop, this));
    }
}

void TcpConnection::ShutdownInLoop() {
    m_loop->CheckInLoopThread();
    if (!m_pChan->IsWriting()) {
        m_pSocket->ShutdownWrite();
    }
}