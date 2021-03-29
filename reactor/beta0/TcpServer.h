#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpConn.h"
#include "InetAddress.h"
#include "netlib/base/noncopyable.h"
#include <unordered_map>

class Acceptor;
class EventLoop;
class IOThreadPool;

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

    void SetThreadNum(int num) {
        m_IoThreadPool->SetThreadNum(num);
    }

    void HandleOneConn(int sockfd, const muduo::InetAddress& peerAddr);
    void RemoveOneConn(const TcpConnPtr& conn);
    void RemoveOneConnInLoop(const TcpConnPtr& conn);
private:
    using ConnMap = std::unordered_map<std::string, TcpConnPtr>;
private:
    EventLoop* m_AcceptorLoop;
    std::unique_ptr<Acceptor> m_acceptor;
    std::unique_ptr<IOThreadPool> m_IoThreadPool;

    TcpConnCallback m_connCallback;
    TcpMsgCallback m_msgCallback;
    WriteCompleteCallback m_writeCompleteCallback;

    bool m_started;
    int m_nextConnId;
    const std::string m_name;
    ConnMap m_connMap;
};

#endif // TCPSERVER_H