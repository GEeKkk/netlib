#ifndef TCPCONN_H
#define TCPCONN_H

#include "foxtail/base/noncopyable.h"
#include <memory>

#include "InetAddress.h"
#include "Callbacks.h"
#include "Socket.h"
#include "Buffer.h"
#include <string_view>


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

    void Send(std::string_view data);
    void Shutdown();

    void ConnEstablished();
    void ConnDestroyed();

    void SetConnCallback(const TcpConnCallback& cb) {
        m_connCallback = cb;
    }

    void SetMsgCallback(const TcpMsgCallback& cb) {
        m_msgCallback = cb;
    }

    void SetCloseCallback(const TcpCloseCallback& cb) {
        m_closeCallback = cb;
    }

    void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
        m_writeCompleteCallback = cb;
    }

    EventLoop* GetLoop() {
        return m_loop;
    }

    std::string name() const {
        return m_name;
    }

    muduo::InetAddress peerAddress() const {
        return m_peerAddr;
    }

    muduo::InetAddress localAddress() const {
        return m_localAddr;
    }

    bool Connected() const {
        return m_state == kConnected;
    }

private:
    enum ConnState {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected,
    };
private:

    void SetState(ConnState st) {
        m_state = st;
    }
    void HandleRead(muduo::Timestamp recvTime);
    void HandleWrite();
    void HandleClose();
    void HandleError();

    void shutInLoop();
    void sendInLoop(std::string_view data);
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
    TcpCloseCallback m_closeCallback;
    WriteCompleteCallback m_writeCompleteCallback;

    Buffer m_inputBuf;
    Buffer m_outputBuf;
};

// using TcpConnPtr = std::shared_ptr<TcpConn>;

#endif // TCPCONN_H