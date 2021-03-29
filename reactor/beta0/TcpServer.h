#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpConn.h"
#include "InetAddress.h"
#include "netlib/base/noncopyable.h"
#include <unordered_map>

class Acceptor;
class EventLoop;

class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop* loop, const muduo::InetAddress& listenAddr);
    ~TcpServer();

    void Start();

    void SetConnCallback(const TcpConnCallback& cb) {
        m_connCallback = cb;
    }
    void SetMsgCallback(const TcpMsgCallback& cb) {
        m_msgCallback = cb;
    }

    void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
        m_writeCompleteCallback = cb;
    }

    void HandleOneConn(int sockfd, const muduo::InetAddress& peerAddr);
    void RemoveOneConn(const TcpConnPtr& conn);
private:
    using ConnMap = std::unordered_map<std::string, TcpConnPtr>;
private:
    EventLoop* m_loop;
    std::unique_ptr<Acceptor> m_acceptor;

    TcpConnCallback m_connCallback;
    TcpMsgCallback m_msgCallback;
    WriteCompleteCallback m_writeCompleteCallback;

    bool m_started;
    int m_nextConnId;
    std::string m_name;
    ConnMap m_connMap;
};

#endif // TCPSERVER_H