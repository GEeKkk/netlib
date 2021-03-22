#include "TcpServer.h"
#include "EventLoop.h"
#include "NetAddr.h"
#include <stdio.h>



std::string message1;
std::string message2;

void onConnection(const TcpConnectionPtr &conn)
{
    if (conn->IsConnected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->GetName().c_str(),
               conn->GetPeerAddr().ToHostPort().c_str());
        conn->Send(message1);
        conn->Send(message2);
        conn->Shutdown();
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

    buf->retrieveAll();
}

int main(int argc, char *argv[])
{
    printf("main(): pid = %d\n", getpid());

    int len1 = 100;
    int len2 = 200;

    if (argc > 2)
    {
        len1 = atoi(argv[1]);
        len2 = atoi(argv[2]);
    }

    message1.resize(len1);
    message2.resize(len2);
    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    NetAddr listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.SetConnHandler(onConnection);
    server.SetMsgHandler(onMessage);
    server.Start();

    loop.Loop();
}