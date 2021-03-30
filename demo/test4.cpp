#include "InetAddress.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "netlib/base/Timestamp.h"

#include <thread>

using namespace std;
using namespace muduo;

void ListenAndLoop(uint16_t port, const Acceptor::ConnHandler& hd) {
    InetAddress listenAddr(port);
    EventLoop loop;
    Acceptor acceptor(&loop, listenAddr);
    acceptor.SetConnHandler(hd);
    acceptor.Listen();
    loop.Loop();
}


int main(int argc, char const *argv[])
{
    printf("main pid = %d\n", getpid());

    thread th1(ListenAndLoop, 9981, ConnectionHandler);
    thread th2(ListenAndLoop, 9982, Response);

    th1.join();
    th2.join();

    return 0;
}
