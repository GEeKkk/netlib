#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "TimerManager.h"

#include "foxtail/base/CurrentThread.h"
#include "foxtail/base/Logging.h"

#include <poll.h>
#include <signal.h>
#include <sys/eventfd.h>

using namespace muduo;
using namespace std;

class IgnoreSigPipe{
public:
    IgnoreSigPipe() {
        signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe IgnoredObj;

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
          m_timerManager(make_unique<TimerQueue>(this)),
          m_wakeupFd(CreateEventfd()),
          m_callPending(false),
          m_wakeupChan(make_unique<Channel>(this, m_wakeupFd))
{
    LOG_DEBUG << "[thread " << m_threadId << "] " << "create loop (" << this << ")";
    /// 每个线程只能有一个eventloop, 
    /// 创建之前需要检查是否已经创建了其他loop对象
    if (t_loopInCurrentThread) {
        LOG_FATAL << "another loop (" << t_loopInCurrentThread 
                  << ") exists in thread [" << m_threadId << "]";
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
        Timestamp evtTime = m_poller->Poll(kPollTimeout, m_activeChans);
        for (auto& it : m_activeChans) {
            it->HandleEvent(evtTime);
        }
        RunStoredFunctors();
    }
    LOG_DEBUG << "(" << this << ") Stop!";
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

void EventLoop::RemoveChannel(Channel* chan) {
    CheckInLoopThread();
    m_poller->RemoveChannel(chan);
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
    LOG_FATAL << "EventLoop must run in IO thread [" 
              << m_threadId 
              << "]. Current thread [" 
              << CurrentThread::tid() 
              << "]. Abort.";
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ssize_t n = write(m_wakeupFd, &one, sizeof(one));
    (void)n;
}

void EventLoop::HandleRead() {
    uint64_t one = 1;
    ssize_t n = read(m_wakeupFd, &one, sizeof(one));
    (void)n;
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


TimerId EventLoop::RunAt(Timestamp& time, const TimerCallback& cb) {
    return m_timerManager->Add(cb, time, 0.0);
}
TimerId EventLoop::RunAfter(double delay, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return RunAt(time, cb);
}
TimerId EventLoop::RunEvery(double interval, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return m_timerManager->Add(cb, time, interval);
}

void EventLoop::StopTimer(TimerId id) {
    m_timerManager->Stop(id);
}