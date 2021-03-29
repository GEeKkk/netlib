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
    m_channel->SetRead(bind(&TcpConn::HandleRead, this, placeholders::_1));
    m_channel->SetWrite(bind(&TcpConn::HandleWrite, this));
    m_channel->SetClose(bind(&TcpConn::HandleClose, this));
    m_channel->SetError(bind(&TcpConn::HandleError, this));
}

TcpConn::~TcpConn() {

}

void TcpConn::Send(string_view data) {
    if (m_state == kConnected) {
        if (m_loop->IsInLoopThread()) {
            sendInLoop(data);
        }
    } else {
        m_loop->RunInLoop(bind(&TcpConn::sendInLoop, this, data));
    }
}

void TcpConn::sendInLoop(string_view msg) {
    m_loop->CheckInLoopThread();
    ssize_t nwrote = 0;
    if (!m_channel->isWriting() && m_outputBuf.readableBytes() == 0) {
        nwrote = write(m_channel->fd(), msg.data(), msg.size());
        if (nwrote >= 0) {
            if (implicit_cast<size_t>(nwrote) < msg.size()) {
                LOG_DEBUG << "STILL MORE DATA NEED TO SEND";
            } else if (m_writeCompleteCallback) {
                m_loop->Stored(bind(m_writeCompleteCallback, shared_from_this()));
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << "TcpConn::sendInLoop";
            }
        }
    }

    if (implicit_cast<size_t>(nwrote) < msg.size()) {
        m_outputBuf.append(msg.data() + nwrote, msg.size() - nwrote);
        if (!m_channel->isWriting()) {
            m_channel->EnableWrite();
        }
    }
}

void TcpConn::shutInLoop() {
    m_loop->CheckInLoopThread();
    if (!m_channel->isWriting()) {
        m_socket->shutdownWrite();
    }
}

void TcpConn::Shutdown() {
    if (m_state == kConnected) {
        SetState(kDisconnecting);
        m_loop->RunInLoop(bind(&TcpConn::shutInLoop, this));
    }
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

void TcpConn::HandleRead(Timestamp recvTime) {
    int Errno = 0;
    ssize_t len = m_inputBuf.readFd(m_channel->fd(), &Errno);
    if (len > 0) {
        m_msgCallback(shared_from_this(), &m_inputBuf, recvTime);
    } else if (len == 0) {
        HandleClose();
    } else {
        HandleError();
    }
}

void TcpConn::HandleWrite() {
    m_loop->CheckInLoopThread();
    if (m_channel->isWriting()) {
        ssize_t n = write(m_channel->fd(), m_outputBuf.peek(), m_outputBuf.readableBytes());
        if (n > 0) {
            m_outputBuf.retrieve(n);
            if (m_outputBuf.readableBytes() == 0) {
                m_channel->DisableWrite();
                if (m_writeCompleteCallback) {
                    m_loop->Stored(bind(m_writeCompleteCallback, shared_from_this()));
                }
                if (m_state == kDisconnecting) {
                    shutInLoop();
                }
            } else {
                LOG_DEBUG << "MORE DATA TO SEND";
            }
        } else {
            LOG_SYSERR << "TcpConn::HandleWrite";
        }
    } else {
        LOG_DEBUG << "Conn is down, no more writing";
    }
}

void TcpConn::HandleClose() {
    m_channel->DisableAll();
    m_closeCallback(shared_from_this());
}

void TcpConn::HandleError() {
    
}