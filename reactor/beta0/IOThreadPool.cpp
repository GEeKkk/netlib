#include "IOThreadPool.h"
#include "EventLoop.h"
#include "IOThread.h"

using namespace std;

IOThreadPool::IOThreadPool(EventLoop* baseLoop) 
    : m_baseLoop(baseLoop),
      m_started(false),
      m_ThreadNum(0),
      m_next(0) 
    {

    }

IOThreadPool::~IOThreadPool() {

}

void IOThreadPool::Start() {
    m_baseLoop->CheckInLoopThread();
    m_started = true;

    for (int i = 0; i < m_ThreadNum; ++i) {
        IOThread* t = new IOThread;
        m_threads.emplace_back(unique_ptr<IOThread>(t));
        m_loops.emplace_back(t->StartLoop());
    }
}

EventLoop* IOThreadPool::GetNextLoop() {
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