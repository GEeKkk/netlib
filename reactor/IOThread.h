#ifndef IOTHREAD_H
#define IOTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "netlib/base/noncopyable.h"

class EventLoop;

class IOThread : noncopyable
{
public:
    IOThread();
    ~IOThread();
    EventLoop* StartLoop();
private:
    void ThreadFunc();
private:
    EventLoop* m_loop;
    bool m_isExit;
    std::thread m_iothread;
    std::mutex m_mu;
    std::condition_variable m_cond;
};

#endif // IOTHREAD_H