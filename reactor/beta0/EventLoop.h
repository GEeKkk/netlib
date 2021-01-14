#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"
#include "netlib/base/CurrentThread.h"
#include <thread>
#include <atomic>

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();

    void AssertInLoopThread() {
        if (!IsInLoopThread()) {
            AbortNotInLoopThread();
        }
    }

    bool IsInLoopThread() const {
        return m_threadId == CurrentThread::tid();
    }
private:
    void AbortNotInLoopThread();
private:
    std::atomic_bool m_looping;
    const pid_t m_threadId;
};



#endif // EVENTLOOP_H