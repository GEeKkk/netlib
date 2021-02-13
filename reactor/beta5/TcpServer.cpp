#include "TcpServer.h"

#include "netlib/base/Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketUtil.h"

#include <functional>
#include <stdio.h>

using namespace muduo;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const NetAddr& listenAddr)
    : m_loop(loop),
      m_name(listenAddr.ToHostPort()),
      m_Acceptor(std::make_unique<Acceptor>(loop, listenAddr)),
      m_IsStarted(false),
      m_NextConnId(1)
{
    m_Acceptor->SetNewConnectionHandler(std::bind(&TcpServer::NewConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::Start() {
    if (!m_IsStarted) {
        m_IsStarted = true;
    }

    if (!m_Acceptor->IsListening()) {
        m_loop->RunInLoop(std::bind(&Acceptor::Listen, m_Acceptor.get()));
    }
}

void TcpServer::SetConnHandler(const ConnectionHandler& hd) {
    m_ConnHandler = hd;
}

void TcpServer::SetMsgHandler(const MessageHandler& hd) {
    m_MsgHandler = hd;
}

void TcpServer::NewConnection(int sockfd, const NetAddr& peerAddr) {
    m_loop->CheckInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", m_NextConnId);
    ++m_NextConnId;

    std::string connName(m_name + buf);

    LOG_DEBUG << "TcpServer::NewConnection [" << m_name 
              << "] - new conn [" << connName 
              << "] from " << peerAddr.ToHostPort();
    
    NetAddr localAddr(SocketUtil::getLocalAddr(sockfd));

    TcpConnectionPtr connPtr(std::make_shared<TcpConnection>(m_loop, 
                                                             connName, 
                                                             sockfd, 
                                                             localAddr, 
                                                             peerAddr));
    m_ConnMap[connName] = connPtr;
    connPtr->SetConnectionHandler(m_ConnHandler);
    connPtr->SetMessageHandler(m_MsgHandler);
    connPtr->ConnectEstablished();
}