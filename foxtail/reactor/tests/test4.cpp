#include "InetAddress.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "netlib/base/Timestamp.h"

#include <thread>

using namespace std;
using namespace muduo;

void ConnectionHandler(Socket&& socket, const InetAddress& peerAddr)
{
    printf("New Conn from %s\n", peerAddr.toHostPort().c_str());
    string curTime = Timestamp::now().toFormattedString();
    curTime.append("\n");
    write(socket.fd(), curTime.data(), curTime.size());
}

void Response(Socket&& sock, const InetAddress& peerAddr) {
    printf("New Con from %s\n", peerAddr.toHostPort().c_str());
    write(sock.fd(), "pong!\n", 6);
}

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
