#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "netlib/base/noncopyable.h"
#include <thread>
#include <atomic>

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
private:
    std::atomic_bool m_looping;
    const pid_t m_threadId;
};



#endif // EVENTLOOP_H