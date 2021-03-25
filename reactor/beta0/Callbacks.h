#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <memory>
#include <functional>

class TcpConn;
using TcpConnPtr = std::shared_ptr<TcpConn>;
using TcpConnCallback = std::function<void(const TcpConnPtr&)>;
using TcpMsgCallback = std::function<void(const TcpConnPtr&, const char* data, ssize_t len)>;
using TcpCloseCallback = std::function<void(const TcpConnPtr&)>;

using TimerCallback = std::function<void()>;

#endif // CALLBACKS_H