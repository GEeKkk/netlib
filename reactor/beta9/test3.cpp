#include "Acceptor.h"
#include "EventLoop.h"
#include "NetAddr.h"
#include "SocketUtil.h"

void NewConnect(int sockfd, const NetAddr& peerAddr)
{
    printf("NewConn: Accept a new connection from %s\n",
           peerAddr.ToHostPort().c_str());
    ::write(sockfd, "Pong!\n", 5);
    SocketUtil::close(sockfd);
}

int main(int argc, char const *argv[])
{
    printf("main pid = %d\n", getpid());

    NetAddr listenAddr(9981);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr);
    acceptor.SetNewConnectionHandler(NewConnect);
    acceptor.Listen();

    loop.Loop();
    return 0;
}
