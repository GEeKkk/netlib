#ifndef TCPCONN_H
#define TCPCONN_H

#include "netlib/base/noncopyable.h"
#include <memory>

#include "InetAddress.h"
#include "Callbacks.h"
#include "Socket.h"

class Channel;
class EventLoop;

class TcpConn : noncopyable,
                public std::enable_shared_from_this<TcpConn>
{
public:
    TcpConn(EventLoop* loop, 
            const std::string& name,
            int sockfd,
            const muduo::InetAddress& local, 
            const muduo::InetAddress& peer);
    ~TcpConn();

    bool Connected() const;
    void SetConnCallback(const TcpConnCallback& cb);
    void SetMsgCallback(const TcpMsgCallback& cb);

    void ConnEstablished();
    std::string name() const {
        return m_name;
    }

    muduo::InetAddress peerAddress() const {
        return m_peerAddr;
    }
private:
    enum ConnState {
        kConnecting,
        kConnected,
    };
private:
    void SetState(ConnState st);
    void HandleRead();
private:
    EventLoop* m_loop;
    std::string m_name;
    ConnState m_state;
    std::unique_ptr<muduo::Socket> m_socket;
    std::unique_ptr<Channel> m_channel;
    muduo::InetAddress m_localAddr;
    muduo::InetAddress m_peerAddr;
    TcpConnCallback m_connCallback;
    TcpMsgCallback m_msgCallback;
};

// using TcpConnPtr = std::shared_ptr<TcpConn>;

#endif // TCPCONN_H