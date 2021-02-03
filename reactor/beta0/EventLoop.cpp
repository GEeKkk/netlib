#include "EventLoop.h"

#include "netlib/base/CurrentThread.h"
#include "netlib/base/Logging.h"

#include <poll.h>

using namespace muduo;

__thread EventLoop* t_loopInCurrentThread = nullptr;

EventLoop::EventLoop()
        : m_looping(false),
          m_threadId(CurrentThread::tid())
{
    LOG_DEBUG << "EventLoop created [" << this << "] in thread (" << m_threadId << ")";
    /// 每个线程只能有一个eventloop, 
    /// 创建之前需要检查是否已经创建了其他loop对象
    if (t_loopInCurrentThread) {
        LOG_FATAL << "Another EventLoop [" << t_loopInCurrentThread 
                  << "] exists in this thread (" << m_threadId << ")";
    } else {
        t_loopInCurrentThread = this;
    }
}

EventLoop::~EventLoop() {
    t_loopInCurrentThread = nullptr;
}

void EventLoop::Loop() {
    // 事件循环必须在IO线程中执行，pre-condition
    CheckInLoopThread();
    m_looping = true;
    ::poll(NULL, 0, 5 * 1000);

    LOG_DEBUG << "EventLoop [" << this << "] stop looping.";
    m_looping = false;
}
void EventLoop::CheckInLoopThread() {
     if (!IsInLoopThread()) {
         AbortNotInLoopThread();
     }
}
bool EventLoop::IsInLoopThread() const {
    return m_threadId == CurrentThread::tid();
}

void EventLoop::AbortNotInLoopThread() {
    LOG_FATAL << "EventLoop Abort. Created in thread(" << m_threadId 
              << "), but current thread is (" << CurrentThread::tid() << ")";
}