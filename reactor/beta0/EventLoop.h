#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"
#include "netlib/base/CurrentThread.h"

#include "TimerId.h"
#include "Callbacks.h"

#include <thread>
#include <atomic>
#include <memory>
#include <vector>

class Channel;
class Poller;
class TimerQueue;

using namespace muduo;

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();

    void Quit();

    Timestamp PollReturnTime() const {
        return m_pollReturnTime;
    }

    TimerId RunAt(const Timestamp& time, const TimerCallback& cb);
    TimerId RunAfter(double delay, const TimerCallback& cb);
    TimerId RunEvery(double interval, const TimerCallback& cb);

    void UpdateChannel(Channel* chan);

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
    using ChanList = std::vector<Channel*>;
    std::atomic_bool m_looping;
    std::atomic_bool m_quit;
    const pid_t m_threadId;
    Timestamp m_pollReturnTime;
    std::unique_ptr<Poller> m_poller;
    std::unique_ptr<TimerQueue> m_TimerQueue;
    ChanList m_activeChans;
};



#endif // EVENTLOOP_H