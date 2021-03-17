#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Handler.h"
#include "TcpConnection.h"
#include "netlib/base/noncopyable.h"

#include <map>
#include <memory>

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop* loop, const NetAddr& listenAddr);
    ~TcpServer();

    void Start();
    void SetThreadNum(int num);
    
    void SetConnHandler(const ConnectionHandler& hd);
    void SetMsgHandler(const MessageHandler& hd);

private:
    void NewConnection(int sockfd, const NetAddr& peerAddr);
    void RemoveConnection(const TcpConnectionPtr& conn);
    void RemoveConnectionInLoop(const TcpConnectionPtr& conn);
private:
    using ConnMap = std::map<std::string, TcpConnectionPtr>;
private:
    EventLoop* m_loop;
    const std::string m_name;
    std::unique_ptr<Acceptor> m_Acceptor;
    std::unique_ptr<EventLoopThreadPool> m_EventLoopThreadPool;

    ConnectionHandler m_ConnHandler;
    MessageHandler m_MsgHandler;

    bool m_IsStarted;
    int m_NextConnId;

    ConnMap m_ConnMap;
};

#endif // TCPSERVER_H