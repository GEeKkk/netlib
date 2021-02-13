#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "netlib/base/noncopyable.h"
#include "NetAddr.h"
#include "Handler.h"

#include <memory>

class Channel;
class EventLoop;
class Socket;

class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const NetAddr& localAddr,
                  const NetAddr& peerAddr);
    ~TcpConnection();

    EventLoop* GetLoop() const;
    const std::string& GetName() const;
    const NetAddr& GetLocalAddr();
    const NetAddr& GetPeerAddr();

    bool IsConnected() const;

    void SetConnectionHandler(const ConnectionHandler& handler);
    void SetMessageHandler(const MessageHandler& handler);

    void ConnectEstablished();

private:
    enum TcpState {
        kConnecting,
        kConnected
    };
private:
    void SetTcpState(TcpState ts);
    void HandleRead();
private:
    EventLoop* m_loop;
    std::string m_name;
    TcpState m_state;
    std::unique_ptr<Socket> m_pSocket;
    std::unique_ptr<Channel> m_pChan;
    NetAddr m_localAddr;
    NetAddr m_peerAddr;
    ConnectionHandler m_connHandler;
    MessageHandler m_msgHandler;
};

#endif // TCPCONNECTION_H