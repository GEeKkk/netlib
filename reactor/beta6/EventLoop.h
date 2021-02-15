#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"

#include <mutex>
#include <vector>
#include <functional>
#include <memory>
#include <unistd.h>

class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;
public:
    EventLoop();
    ~EventLoop();

    void Loop();

    void Quit();

    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);

    void CheckInLoopThread();

    bool IsInLoopThread() const;

    void queueInLoop(const Functor& func);
    void RunInLoop(const Functor& func);

    void WakeUp();

private:
    void AbortNotInLoopThread();
    void DoPendingFuncs();
    void HandleRead();
private:
    using ChanVec = std::vector<Channel*>;
private:
    bool m_looping;
    bool m_quit;
    bool m_callPendingFuncs;
    const pid_t m_threadId; // 创建了EventLoop对象的线程是IO线程, 主要功能是运行Loop()
    std::unique_ptr<Poller> m_poller;
    int m_wakeupFd;
    std::unique_ptr<Channel> m_wakeupChan;
    ChanVec m_ActiveChannels;
    std::mutex m_mutex;
    std::vector<Functor> m_pendingFuncs;
};

#endif // EVENTLOOP_H