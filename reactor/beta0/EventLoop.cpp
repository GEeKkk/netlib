#include "EventLoop.h"
#include "netlib/base/Logging.h"
#include <poll.h>

__thread EventLoop* t_loopInThisThread = NULL;

EventLoop::EventLoop() 
    : m_looping(false),
      m_threadId(CurrentThread::tid())
{
    LOG_DEBUG << "EventLoop created [" << this << "] in thread [" << m_threadId << "]";
    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread 
                  << " exists in current thread [" << m_threadId << "]";
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    t_loopInThisThread = NULL;
}

void EventLoop::Loop()
{
    AssertInLoopThread();
    m_looping = true;
    
    ::poll(NULL, 0, 5 * 1000);

    LOG_DEBUG << "EventLoop [" << this << "] stop looping";
    m_looping = false;
}

void EventLoop::AbortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::AbortNotInLoopThread - EventLoop " << this
            << " was created in m_threadId = " << m_threadId
            << ", current thread id = " <<  CurrentThread::tid();
}