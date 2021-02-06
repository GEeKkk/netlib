#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

#include "netlib/base/noncopyable.h"
#include "Channel.h"
#include "Socket.h"

class EventLoop;
class NetAddr;

class Acceptor : noncopyable
{
public:
    using NewConnectionHandler = std::function<void(int sockfd, const NetAddr&)>;
public:
    Acceptor(EventLoop* loop, const NetAddr& listenAddr);

    void SetNewConnectionHandler(const NewConnectionHandler& cb);

    bool IsListening() const;

    void Listen();

private:
    void HandleRead();
private:
    EventLoop* m_loop;
    Socket m_AcceptSock;
    Channel m_AcceptChannel;
    NewConnectionHandler m_newConnHandler;
    bool m_listening;
};

#endif // ACCEPTOR_H