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
    using ConnHandler = std::function<void(int sockfd,
                                           const muduo::InetAddress& addr)>;

public:
    Acceptor(EventLoop* loop, const muduo::InetAddress& listenAddr);
    ~Acceptor() {}

    void SetConnHandler(const ConnHandler& hd);
    void Listen();

    bool IsListenning() const;

private:
    void HandleRead();
private:
    EventLoop* m_loop;
    muduo::Socket m_AcceptSocket;
    Channel m_AcceptChannel;
    ConnHandler m_ConnHandler;
    bool m_listenning;
};

#endif // ACCEPTOR_H