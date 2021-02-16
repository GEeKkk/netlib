#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <mutex>
#include <thread>
#include <condition_variable>

#include "netlib/base/noncopyable.h"

class EventLoop;

class EventLoopThread : noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* StartLoop();
private:
    void ThreadFunc();
private:
    EventLoop* m_loop;
    bool m_exit;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};


#endif // EVENTLOOPTHREAD_H