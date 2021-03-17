#ifndef HANDLER_H
#define HANDLER_H

#include <memory>
#include <functional>
#include "Buffer.h"
#include "netlib/base/Timestamp.h"

class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionHandler = std::function<void(const TcpConnectionPtr&)>;
using MessageHandler = std::function<void(const TcpConnectionPtr&,
                                          muduo::Buffer* buf,
                                          muduo::Timestamp)>;
using CloseHandler = std::function<void(const TcpConnectionPtr&)>;
using WriteDoneHandler = std::function<void(const TcpConnectionPtr&)>;

#endif // HANDLER_H