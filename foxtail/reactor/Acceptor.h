#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

#include "Channel.h"
#include "Socket.h"
#include "InetAddress.h"
#include "foxtail/base/noncopyable.h"

class EventLoop;

class Acceptor : noncopyable
{
public:
    using ConnCallback = std::function<void(int sockfd,
                                           const muduo::InetAddress& addr)>;

public:
    Acceptor(EventLoop* loop, const muduo::InetAddress& listenAddr);
    ~Acceptor() {}

    void Listen();

    void SetConnCallback(const ConnCallback& cb) {
        m_ConnCallback = cb;
    }
    bool IsListenning() const {
        return m_listenning;
    }

private:
    void HandleRead();
private:
    EventLoop* m_loop;
    muduo::Socket m_AcceptSocket;
    Channel m_AcceptChannel;
    ConnCallback m_ConnCallback;
    bool m_listenning;
};

#endif // ACCEPTOR_H