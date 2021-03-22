#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include "netlib/base/noncopyable.h"

#include "Channel.h"
#include "Socket.h"
#include "InetAddress.h"

class EventLoop;

class Acceptor : noncopyable
{
public:
    using ConnectionHandler = std::function<void(muduo::Socket&& socket,
                                                const muduo::InetAddress& addr)>;

public:
    Acceptor(EventLoop* loop, const muduo::InetAddress& listenAddr);
    ~Acceptor() {}

    void SetConnectionHandler(const ConnectionHandler& hd);
    void Listen();

private:
    void HandleRead();
private:
    EventLoop* m_loop;
    muduo::Socket m_AcceptSocket;
    Channel m_AcceptChannel;
    ConnectionHandler m_ConnHandler;
};

#endif // ACCEPTOR_H