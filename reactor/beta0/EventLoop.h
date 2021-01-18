#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"
#include "netlib/base/CurrentThread.h"
#include <thread>
#include <atomic>
#include <memory>
#include <vector>

class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();

    void Quit();

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
    std::unique_ptr<Poller> m_poller;
    ChanList m_activeChans;
};



#endif // EVENTLOOP_H