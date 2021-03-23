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

    void SetConnCallback(const TcpConnCallback& cb);
    void SetMsgCallback(const TcpMsgCallback& cb);

    void HandleOneConn(int sockfd, const muduo::InetAddress& peerAddr);
private:
    using ConnMap = std::unordered_map<std::string, TcpConnPtr>;
private:
    EventLoop* m_loop;
    std::unique_ptr<Acceptor> m_acceptor;
    TcpConnCallback m_connCallback;
    TcpMsgCallback m_msgCallback;
    bool m_started;
    int m_nextConnId;
    std::string m_name;
    ConnMap m_connMap;
};

#endif // TCPSERVER_H