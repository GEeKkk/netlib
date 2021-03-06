#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"
#include "Callbacks.h"
// #include ""

#include <unistd.h>
#include <memory>
#include <vector>
#include <mutex>

class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    void UpdateChannel(Channel* chan);
    void RemoveChannel(Channel* chan);

    void CheckInLoopThread();
    bool IsInLoopThread() const;

    void RunInLoop(const Functor& cb);
    void Stored(const Functor& cb);

    TimerId RunAt(muduo::Timestamp& time, const TimerCallback& cb);
    TimerId RunAfter(double delay, const TimerCallback& cb);
    TimerId RunEvery(double interval, const TimerCallback& cb);
    void StopTimer(TimerId id);
private:
    void AbortNotInLoopThread();
    void HandleRead();
    void RunStoredFunctors();
    void WakeUp();
private:
    bool m_looping;
    const pid_t m_threadId; // 创建了EventLoop对象的线程是IO线程, 主要功能是运行Loop()
    bool m_quit;
    std::unique_ptr<Poller> m_poller;
    std::vector<Channel*> m_activeChans;
    std::unique_ptr<TimerQueue> m_timerManager;
    int m_wakeupFd;
    bool m_callPending;
    std::unique_ptr<Channel> m_wakeupChan;
    std::vector<Functor> m_pendings;
    std::mutex m_mu;
};

#endif // EVENTLOOP_H