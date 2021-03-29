#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "netlib/base/CurrentThread.h"
#include "netlib/base/Timestamp.h"
#include <stdio.h>
using namespace std;
using namespace muduo;
std::string message;

void onConnection(const TcpConnPtr &conn)
{
    if (conn->Connected())
    {
        printf("onConnection(): tid=%d new connection [%s] from %s\n",
               CurrentThread::tid(),
               conn->name().c_str(),
               conn->peerAddress().toHostPort().c_str());
        conn->Send(message);
    }
    else
    {
        printf("onConnection(): tid=%d connection [%s] is down\n",
               CurrentThread::tid(),
               conn->name().c_str());
    }
}

void onWriteComplete(const TcpConnPtr &conn)
{
    conn->Send(message);
}

void onMessage(const TcpConnPtr &conn,
               Buffer *buf,
               Timestamp receiveTime)
{
    printf("onMessage(): tid=%d received %zd bytes from connection [%s] at %s\n",
           CurrentThread::tid(),
           buf->readableBytes(),
           conn->name().c_str(),
           receiveTime.toFormattedString().c_str());

    buf->retrieveAll();
}

int main(int argc, char *argv[])
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

    InetAddress listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.SetConnCallback(onConnection);
    server.SetMsgCallback(onMessage);
    server.SetWriteCompleteCallback(onWriteComplete);
    if (argc > 1)
    {
        server.SetThreadNum(atoi(argv[1]));
    }
    server.Start();

    loop.Loop();
}
