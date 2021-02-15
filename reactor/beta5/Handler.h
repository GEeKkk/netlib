#ifndef HANDLER_H
#define HANDLER_H

#include <memory>
#include <functional>
#include "Buffer.h"

class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionHandler = std::function<void(const TcpConnectionPtr&)>;
using MessageHandler = std::function<void(const TcpConnectionPtr&,
                                          
                                          ssize_t len)>;
using CloseHandler = std::function<void(const TcpConnectionPtr&)>;

#endif // HANDLER_H