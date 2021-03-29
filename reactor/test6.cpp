#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "netlib/base/Timestamp.h"
#include <stdio.h>

void onConnection(const TcpConnPtr &conn)
{
    if (conn->Connected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->name().c_str(),
               conn->peerAddress().toHostPort().c_str());
        sleep(20);
        conn->Send("Shit\n");
        conn->Send("shit2\n");
        conn->Shutdown();
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
               conn->name().c_str());
    }
}

void onMessage(const TcpConnPtr &conn,
               Buffer *buf,
               muduo::Timestamp recvTime)
{
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           buf->readableBytes(), conn->name().c_str());
    
    printf("data: %s, time %s\n",buf->retrieveAsString().c_str(), recvTime.toFormattedString().c_str());
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    muduo::InetAddress listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.SetConnCallback(onConnection);
    server.SetMsgCallback(onMessage);
    server.Start();

    loop.Loop();
}
