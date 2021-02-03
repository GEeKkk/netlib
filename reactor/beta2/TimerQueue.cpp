#include "netlib/base/Logging.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include "TimerQueue.h"

#include <functional>
#include <sys/timerfd.h>
#include <unistd.h>

namespace TimerFdOperation
{
    int CreateTimerfd() {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                        TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd < 0) {
            LOG_SYSFATAL << "Failed in timerfd_create";
        }
        return timerfd;
    }

    struct timespec DurationFromNow(Timestamp when) {
        int64_t msec = when.microSecondsSinceEpoch()
                        - Timestamp::now().microSecondsSinceEpoch();
        
        if (msec < 100) {
            msec = 100;
        }

        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(msec / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>((msec % Timestamp::kMicroSecondsPerSecond) * 1000);
        return ts;
    }

    void ReadTimerFd(int timerfd, Timestamp now) {
        uint64_t howmany;
        ssize_t nBytes = ::read(timerfd, & howmany, sizeof(howmany));
        if (nBytes != sizeof(howmany)) {
            LOG_ERROR << "TimerQueue::HandRead() reads " << nBytes << " bytes rather than 8 bytes.";
        }
    }

    void ResetTimerFd(int timerfd, Timestamp expiration) {
        struct itimerspec newVal;
        struct itimerspec oldVal;
        bzero(&newVal, sizeof(newVal));
        bzero(&oldVal, sizeof(oldVal));
        newVal.it_value = DurationFromNow(expiration);
        int ret = ::timerfd_settime(timerfd, 0, &newVal, &oldVal);
        if (ret) {
            LOG_SYSERR << "timerfd_settime()";
        }
    }
}

using namespace TimerFdOperation;


TimerQueue::TimerQueue(EventLoop* loop)
    : m_loop(loop),
      m_timerfd(CreateTimerfd()),
      m_timerfdChan(loop, m_timerfd),
      m_timerSet()
    {
        m_timerfdChan.SetReadCallback(std::bind(&TimerQueue::HandleRead, this));
        m_timerfdChan.EnableReading();
    }

TimerQueue::~TimerQueue() 
{
    ::close(m_timerfd);
    for (auto& it : m_timerSet) {
        delete it.second;
    }
}

TimerId TimerQueue::AddTimer(const TimerCallback& cb,
                             Timestamp when,
                             double interval)
{
    auto timer = new Timer(cb, when, interval);

    m_loop->AssertInLoopThread();

    bool earliestChanged = Insert(timer);

    if (earliestChanged) {
        ResetTimerFd(m_timerfd, timer->GetExpiration());
    }

    return TimerId(timer);
}

void TimerQueue::HandleRead() 
{
    m_loop->AssertInLoopThread();
    Timestamp now(Timestamp::now());
    ReadTimerFd(m_timerfd, now);

    std::vector<Entry> expired = GetExpired(now);
    for (auto& it : expired) {
        it.second->Run();
    }

    Reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::GetExpired(Timestamp now)
{
    std::vector<Entry> expired;
    auto sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto it = m_timerSet.lower_bound(sentry);
    std::copy(m_timerSet.begin(), it, std::back_inserter(expired));
    m_timerSet.erase(m_timerSet.begin(), it);

    return expired;
}


void TimerQueue::Reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;
    for (auto& it : expired) {
        if (it.second->IsRepeat()) {
            it.second->ReStart(now);
            Insert(it.second);
        } else {
            delete it.second;
        }
    }

    if (!m_timerSet.empty()) {
        nextExpire = m_timerSet.begin()->second->GetExpiration();
    }

    if (nextExpire.valid()) {
        ResetTimerFd(m_timerfd, nextExpire);
    }
}

bool TimerQueue::Insert(Timer* timer)
{
    bool earliestChanged = false;
    Timestamp when = timer->GetExpiration();
    auto it = m_timerSet.begin();
    if (it == m_timerSet.end() || when < it->first)
    {
        earliestChanged = true;
    }

    m_timerSet.emplace(Entry(when, timer));

    return earliestChanged;
}