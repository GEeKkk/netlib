#include "EventLoopThread.h"
#include "EventLoop.h"

#include <functional>


EventLoopThread::EventLoopThread()
    : m_loop(nullptr),
      m_exit(false)
{
}

EventLoopThread::~EventLoopThread() {
    m_exit = true;
    m_loop->Quit();
    m_thread.join();
}

EventLoop* EventLoopThread::StartLoop() {
    m_thread = std::thread(std::bind(&EventLoopThread::ThreadFunc, this));

    {
        std::unique_lock<std::mutex> unqlock(m_mutex);
        while (m_loop == nullptr)
        {
            m_cond.wait(unqlock);
        }
    }

    return m_loop;
}

void EventLoopThread::ThreadFunc() {
    EventLoop loop;

    {
        std::lock_guard<std::mutex> lockguard(m_mutex);
        m_loop = &loop;
        m_cond.notify_one();
    }

    loop.Loop();
}