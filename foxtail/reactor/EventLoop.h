#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"
#include "Callbacks.h"

#include <unistd.h>
#include <memory>
#include <vector>
#include <mutex>

class Channel;
class Poller;
class TimerManager;

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    void UpdateChannel(Channel* chan);
    void RemoveChannel(Channel* chan);

    void CheckInLoopThread();
    bool IsInLoopThread() const;

    using Functor = std::function<void()>;
    void RunInLoop(const Functor& cb);
    void Stored(const Functor& cb);

    void RunAt(const muduo::Timestamp& time, const TimerCallback& cb);
    void RunAfter(double delay, const TimerCallback& cb);
    void RunEvery(double interval, const TimerCallback& cb);
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
    std::unique_ptr<TimerManager> m_timerManger;
    std::vector<Channel*> m_activeChans;
    int m_wakeupFd;
    bool m_callPending;
    std::unique_ptr<Channel> m_wakeupChan;
    std::vector<Functor> m_pendings;
    std::mutex m_mu;
};

#endif // EVENTLOOP_H