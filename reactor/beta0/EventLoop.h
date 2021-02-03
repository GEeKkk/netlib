#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"

#include <unistd.h>

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void CheckInLoopThread();
    bool IsInLoopThread() const;

private:
    void AbortNotInLoopThread();
private:
    bool m_looping;
    const pid_t m_threadId; // 创建了EventLoop对象的线程是IO线程, 主要功能是运行Loop()
};

#endif // EVENTLOOP_H