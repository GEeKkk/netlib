#include "echo.h"
#include <foxtail/reactor/EventLoop.h>
#include <unistd.h>

using namespace muduo;

int main()
{
    EventLoop loop;
    muduo::InetAddress listenAddr(19970);
    EchoServer server(&loop, listenAddr);
    server.Start();
    loop.Loop();
}
