#ifndef ECHO_H
#define ECHO_H

#include <foxtail/reactor/TcpServer.h>

class EchoServer
{
public:
    EchoServer(EventLoop* loop, const muduo::InetAddress& listenAddr);
    void Start();
private:
    void OnConn(const TcpConnPtr& conn);
    void OnMessage(const TcpConnPtr& conn, 
                   Buffer* buf,
                   muduo::Timestamp time);
private:
    TcpServer m_server;
};

#endif // ECHO_H