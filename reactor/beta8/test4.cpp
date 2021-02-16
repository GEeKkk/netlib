#include "TcpServer.h"
#include "EventLoop.h"
#include "NetAddr.h"
#include "Handler.h"
#include "Buffer.h"

#include <stdio.h>


void onConnection(const TcpConnectionPtr& conn) {
    if (conn->IsConnected()) {
        printf("New connection [%s] from %s\n", 
               conn->GetName().c_str(),
               conn->GetPeerAddr().ToHostPort().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn,
               muduo::Buffer* data,
               muduo::Timestamp) 
{
    printf("from connection [%s]\n",
           conn->GetName().c_str());
}

int main(int argc, char const *argv[])
{
    printf("main: pid = %d\n", getpid());

    NetAddr listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.SetConnHandler(onConnection);
    server.SetMsgHandler(onMessage);
    server.Start();

    loop.Loop();
    return 0;
}
