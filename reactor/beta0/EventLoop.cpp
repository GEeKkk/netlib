#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "TimerQueue.h"

#include "netlib/base/Logging.h"
#include <poll.h>

__thread EventLoop* t_loopInThisThread = NULL;
const int kPollTimeMs = 10000;

EventLoop::EventLoop() 
    : m_looping(false),
      m_quit(false),
      m_threadId(CurrentThread::tid()),
      m_poller(std::make_unique<Poller>(this)),
      m_TimerQueue(std::make_unique<TimerQueue>(this))
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
    m_quit = false;

    while (!m_quit)
    {
        m_activeChans.clear();
        m_poller->Poll(kPollTimeMs, &m_activeChans);

        for (auto& it : m_activeChans) {
            it->HandleEvent();
        }

        LOG_DEBUG << "EventLoop [" << this << "] stop looping";
        m_looping = false;
    }
}

void EventLoop::Quit() 
{
    m_quit = true;
}

TimerId EventLoop::RunAt(const Timestamp& time, const TimerCallback& cb)
{
    return m_TimerQueue->AddTimer(cb, time, 0.0);
}

TimerId EventLoop::RunAfter(double delay, const TimerCallback& cb) 
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return RunAt(time, cb);
}

TimerId EventLoop::RunEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return m_TimerQueue->AddTimer(cb, time, interval);
}

void EventLoop::UpdateChannel(Channel* chan)
{
    AssertInLoopThread();
    m_poller->UpdateChannel(chan);
}

void EventLoop::AbortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::AbortNotInLoopThread - EventLoop " << this
            << " was created in m_threadId = " << m_threadId
            << ", current thread id = " <<  CurrentThread::tid();
}