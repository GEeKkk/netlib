#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "TimerManager.h"

#include "netlib/base/CurrentThread.h"
#include "netlib/base/Logging.h"

#include <poll.h>
#include <sys/eventfd.h>

using namespace muduo;
using namespace std;

static int CreateEventfd() {
    int evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evfd < 0) {
        abort();
    }
    return evfd;
}

const int kPollTimeout = 10000;
__thread EventLoop* t_loopInCurrentThread = nullptr;

EventLoop::EventLoop()
        : m_looping(false),
          m_threadId(CurrentThread::tid()),
          m_quit(false),
          m_poller(make_unique<Poller>(this)),
          m_timerManger(make_unique<TimerManager>(this)),
          m_wakeupFd(CreateEventfd()),
          m_callPending(false),
          m_wakeupChan(make_unique<Channel>(this, m_wakeupFd))
{
    LOG_DEBUG << "Created loop [" << this << "] in thread " << m_threadId;
    /// 每个线程只能有一个eventloop, 
    /// 创建之前需要检查是否已经创建了其他loop对象
    if (t_loopInCurrentThread) {
        LOG_FATAL << "Another EventLoop [" << t_loopInCurrentThread 
                  << "] exists in this thread (" << m_threadId << ")";
    } else {
        t_loopInCurrentThread = this;
    }

    m_wakeupChan->SetRead(std::bind(&EventLoop::HandleRead, this));
    m_wakeupChan->EnableRead();
}

EventLoop::~EventLoop() {
    t_loopInCurrentThread = nullptr;
    close(m_wakeupFd);
}

void EventLoop::Loop() {
    // 事件循环必须在IO线程中执行，pre-condition
    CheckInLoopThread();
    m_looping = true;
    m_quit = false;
    while (!m_quit) {
        m_activeChans.clear();
        m_poller->Poll(kPollTimeout, m_activeChans);
        for (auto& it : m_activeChans) {
            it->HandleEvent();
        }
        RunStoredFunctors();
    }
    LOG_DEBUG << "[" << this << "] STOP!";
    m_looping = false;
}

void EventLoop::Quit() {
    m_quit = true;
    if (!IsInLoopThread()) {
        WakeUp();
    }
}

void EventLoop::UpdateChannel(Channel* chan) {
    CheckInLoopThread();
    m_poller->UpdateChannel(chan);
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
    LOG_FATAL << "EventLoop Abort. Created in thread(" 
              << m_threadId 
              << "), but current thread is (" 
              << CurrentThread::tid() 
              << ")";
}

void EventLoop::RunAt(const Timestamp& time, const TimerCallback& cb) {
    m_timerManger->AddTimer(cb, time, 0.0);
}

void EventLoop::RunAfter(double delay, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    RunAt(time, cb);
}

void EventLoop::RunEvery(double interval, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    m_timerManger->AddTimer(cb, time, interval);
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    write(m_wakeupFd, &one, sizeof(one));
}

void EventLoop::HandleRead() {
    uint64_t one = 1;
    read(m_wakeupFd, &one, sizeof(one));
}

void EventLoop::RunInLoop(const Functor& cb) {
    if (IsInLoopThread()) {
        cb();
    } else {
        Stored(cb);
    }
}

void EventLoop::Stored(const Functor& cb) {
    {
        lock_guard<mutex> guard(m_mu);
        m_pendings.emplace_back(cb);
    }
    if (!IsInLoopThread() || m_callPending) {
        WakeUp();
    }
}

void EventLoop::RunStoredFunctors() {
    vector<Functor> functors;
    m_callPending = true;

    {
        lock_guard<mutex> guard(m_mu);
        functors.swap(m_pendings);
    }

    for (auto& cb : functors) {
        cb();
    }

    m_callPending = false;
}