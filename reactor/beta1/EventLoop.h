#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"

#include <vector>
#include <memory>
#include <unistd.h>

class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();

    void Quit();

    void UpdateChannel(Channel* channel);

    void CheckInLoopThread();

    bool IsInLoopThread() const;
private:
    void AbortNotInLoopThread();
private:
    bool m_looping;
    bool m_quit;
    const pid_t m_threadId; // 创建了EventLoop对象的线程是IO线程, 主要功能是运行Loop()
    std::unique_ptr<Poller> m_poller;
    std::vector<Channel*> m_ActiveChannels;
};

#endif // EVENTLOOP_H