#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

#include "netlib/base/noncopyable.h"

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void SetThreadNum(int num);
    void Start();
    EventLoop* GetNextLoop();
private:
    EventLoop* m_baseLoop;
    bool m_started;
    int m_numThreads;
    int m_next;
    std::vector<std::unique_ptr<EventLoopThread>> m_vthreads;
    std::vector<EventLoop*> m_loops;
};


#endif // EVENTLOOPTHREADPOOL_H