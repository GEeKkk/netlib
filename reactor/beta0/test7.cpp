#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <stdio.h>

using namespace muduo;

std::string message;

void onConnection(const TcpConnPtr &conn)
{
    if (conn->Connected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->name().c_str(),
               conn->peerAddress().toHostPort().c_str());
        conn->Send(message);
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
               conn->name().c_str());
    }
}

void onWriteComplete(const TcpConnPtr &conn)
{
    printf("shitbro\n");
    conn->Send(message);
}

void onMessage(const TcpConnPtr &conn,
               Buffer *buf,
               Timestamp receiveTime)
{
    printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
           buf->readableBytes(),
           conn->name().c_str(),
           receiveTime.toFormattedString().c_str());

    buf->retrieveAll();
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    std::string line;
    for (int i = 33; i < 127; ++i)
    {
        line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127 - 33; ++i)
    {
        message += line.substr(i, 72) + '\n';
    }
    printf("line: [%s]", message.c_str());

    InetAddress listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.SetConnCallback(onConnection);
    server.SetMsgCallback(onMessage);
    // server.SetWriteCompleteCallback(onWriteComplete);
    server.Start();

    loop.Loop();
}
