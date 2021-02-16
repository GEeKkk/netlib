#include "EventLoopThreadPool.h"

#include "EventLoop.h"
#include "EventLoopThread.h"

#include <functional>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop)
    : m_baseLoop(baseloop),
      m_started(false),
      m_numThreads(0),
      m_next(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::Start() {
    m_baseLoop->CheckInLoopThread();

    m_started = true;

    for (int i = 0; i < m_numThreads; ++i) {
        EventLoopThread* t = new EventLoopThread;
        m_vthreads.emplace_back(t);
        m_loops.emplace_back(t->StartLoop());
    }
}

void EventLoopThreadPool::SetThreadNum(int num) {
    m_numThreads = num;
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
    m_baseLoop->CheckInLoopThread();

    EventLoop* loop = m_baseLoop;
    if (!m_loops.empty()) {
        loop = m_loops[m_next];
        ++m_next;
        if (static_cast<size_t>(m_next) >= m_loops.size()) {
            m_next = 0;
        }
    }

    return loop;
}