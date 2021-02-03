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
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();

    void Loop();

    void Quit();

    Timestamp PollReturnTime() const {
        return m_pollReturnTime;
    }

    void RunInLoop(const Functor& cb);
    void QueueInLoop(const Functor& cb);

    TimerId RunAt(const Timestamp& time, const TimerCallback& cb);
    TimerId RunAfter(double delay, const TimerCallback& cb);
    TimerId RunEvery(double interval, const TimerCallback& cb);

    void UpdateChannel(Channel* chan);
    void WakeUp();
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
    void HandleRead();
    void DoPendingFunctors();
private:
    using ChanList = std::vector<Channel*>;
    std::atomic_bool m_looping;
    std::atomic_bool m_quit;
    std::atomic_bool m_callingPending;
    const pid_t m_threadId;
    int m_wakeupfd;
    Timestamp m_pollReturnTime;
    std::unique_ptr<Channel> m_wakeupChan;
    std::unique_ptr<Poller> m_poller;
    std::unique_ptr<TimerQueue> m_TimerQueue;
    ChanList m_activeChans;
    std::vector<Functor> m_vPendingFunctors;
};



#endif // EVENTLOOP_H