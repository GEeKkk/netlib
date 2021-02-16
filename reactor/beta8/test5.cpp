#include "TcpServer.h"
#include "EventLoop.h"
#include "NetAddr.h"
#include "Buffer.h"

#include <stdio.h>

void onConnection(const TcpConnectionPtr &conn)
{
    if (conn->IsConnected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->GetName().c_str(),
               conn->GetPeerAddr().ToHostPort().c_str());
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
               conn->GetName().c_str());
    }
}

void onMessage(const TcpConnectionPtr &conn,
               muduo::Buffer *buf,
               muduo::Timestamp receiveTime)
{
    printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
           buf->readableBytes(),
           conn->GetName().c_str(),
           receiveTime.toFormattedString().c_str());

    printf("onMessage(): [%s]\n", buf->retrieveAsString().c_str());
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    NetAddr listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.SetConnHandler(onConnection);
    server.SetMsgHandler(onMessage);
    server.Start();

    loop.Loop();
}