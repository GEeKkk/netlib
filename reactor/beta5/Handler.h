#ifndef HANDLER_H
#define HANDLER_H

#include <memory>
#include <functional>

class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionHandler = std::function<void(const TcpConnectionPtr&)>;
using MessageHandler = std::function<void(const TcpConnectionPtr&,
                                          const char* data,
                                          ssize_t len)>;
using CloseHandler = std::function<void(const TcpConnectionPtr&)>;

#endif // HANDLER_H