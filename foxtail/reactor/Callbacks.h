#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <memory>
#include <functional>

#include "foxtail/base/Timestamp.h"

class Buffer;
class TcpConn;
using TcpConnPtr = std::shared_ptr<TcpConn>;
using TcpConnCallback = std::function<void(const TcpConnPtr&)>;
using TcpMsgCallback = std::function<void(const TcpConnPtr&, Buffer* data, muduo::Timestamp)>;
using TcpCloseCallback = std::function<void(const TcpConnPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnPtr&)>;

using TimerCallback = std::function<void()>;
using TimerId = int64_t;

#endif // CALLBACKS_H