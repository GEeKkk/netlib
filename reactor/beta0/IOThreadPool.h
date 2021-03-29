#ifndef IOTHREADPOOL_H
#define IOTHREADPOOL_H

#include <mutex>
#include <thread>
#include <condition_variable>

#include <vector>
#include <memory>

#include "netlib/base/noncopyable.h"

class EventLoop;
class IOThread;

class IOThreadPool : noncopyable
{
public:
    IOThreadPool(EventLoop* baseLoop);
    ~IOThreadPool();

    void Start();
    EventLoop* GetNextLoop();

    void SetThreadNum(int num) {
        m_ThreadNum = num;
    }
private:
    EventLoop* m_baseLoop;
    bool m_started;
    int m_ThreadNum;
    int m_next;
    std::vector<std::unique_ptr<IOThread>> m_threads;
    std::vector<EventLoop*> m_loops;
};

#endif // IOTHREADPOOL_H