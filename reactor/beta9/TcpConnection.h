#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "netlib/base/noncopyable.h"
#include "netlib/base/Timestamp.h"
#include "NetAddr.h"
#include "Handler.h"
#include "Buffer.h"

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

    void Send(const std::string& message);
    void Shutdown();

    void SetConnectionHandler(const ConnectionHandler& handler);
    void SetMessageHandler(const MessageHandler& handler);
    void SetWriteDoneHandler(const WriteDoneHandler& handler);

    // internal use
    void SetCloseHandler(const CloseHandler& handler);

    void ConnectEstablished();
    void ConnectDestroyed();

private:
    enum TcpState {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    };
private:
    void SetTcpState(TcpState ts);

    void HandleRead(muduo::Timestamp recvTime);
    void HandleWrite();
    void HandleError();
    void HandleClose();

    void SendInLoop(const std::string& message);
    void ShutdownInLoop();
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
    CloseHandler m_closeHandler;
    WriteDoneHandler m_writeDoneHandler;

    muduo::Buffer m_inputBuffer;
    muduo::Buffer m_ouputBuffer;
};

#endif // TCPCONNECTION_H