#include "echo.h"

using namespace std;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

EchoServer::EchoServer(EventLoop* loop, const muduo::InetAddress& listenAddr)
    : m_server(loop, listenAddr)
{
    m_server.SetConnCallback(bind(&EchoServer::OnConn, this, ::_1));
    m_server.SetMsgCallback(bind(&EchoServer::OnMessage, this, ::_1, ::_2, ::_3));
}

void EchoServer::Start() {
    m_server.Start();
}

void EchoServer::OnConn(const TcpConnPtr& conn)
{
  LOG_DEBUG << conn->peerAddress().toHostPort() << " -> " << conn->localAddress().toHostPort() << (conn->Connected() ? " UP" : " DOWN");
}

void EchoServer::OnMessage(const TcpConnPtr& conn,
                           Buffer* buf,
                           muduo::Timestamp time)
{
  string msg(buf->retrieveAsString());
  // LOG_DEBUG << conn->name() << " echo " << msg.size() << " bytes, "
  //          << "data received at " << time.toFormattedString();
  conn->Send(msg);
}
