#include "TimerManager.h"
#include  "EventLoop.h"

#include <sys/timerfd.h>
#include <string.h>

#include "foxtail/base/Logging.h"

using namespace muduo;
using namespace std;

namespace foxdetail 
{
int CreateTimerfd() {
    return timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
}

void ReadTimerfd(int fd) {
    uint64_t buf;
    ssize_t n = read(fd, &buf, sizeof(buf));
    (void)n;
}

struct timespec DurationFromNow(Timestamp finishTime) {
    int64_t elapsedMs = finishTime.microSecondsSinceEpoch() 
                    - Timestamp::now().microSecondsSinceEpoch();
    if (elapsedMs < 100) {
        elapsedMs = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(elapsedMs / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((elapsedMs % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void StartTimer(int timerfd, Timestamp finishTime){
    struct itimerspec newValue;
    bzero(&newValue, sizeof(newValue));
    newValue.it_value = DurationFromNow(finishTime);
    timerfd_settime(timerfd, 0, &newValue, NULL);
}
}
using namespace foxdetail;

TimerManager::TimerManager(EventLoop* loop)
    : m_loop(loop),
      m_timerfd(CreateTimerfd()),
      m_timerChan(loop, m_timerfd)
    {
        m_timerChan.SetRead(bind(&TimerManager::HandleRead, this));
        m_timerChan.EnableRead();
    }

TimerManager::~TimerManager() {
    close(m_timerfd);
}

void TimerManager::AddTimer(const TimerCallback& cb,
                            Timestamp when,
                            double interval)
{
    bool isEarliest = Insert(make_unique<Timer>(cb, when, interval));
    if (isEarliest) {
        StartTimer(m_timerfd, when);
    }
}

void TimerManager::HandleRead() {
    ReadTimerfd(m_timerfd);
    Timestamp now = Timestamp::now();
    auto expired = GetExpiredTimers(now);
    for (auto& timer : expired) {
        timer->Run();
    }
    Reset(expired, now);
}

vector<unique_ptr<Timer>> TimerManager::GetExpiredTimers(Timestamp now) {
    vector<unique_ptr<Timer>> expired;
    auto it = m_timers.lower_bound(make_unique<Timer>(TimerCallback(), now, 0.0));
    for (auto itor = m_timers.begin(); itor != it; ) {
        expired.emplace_back(move(m_timers.extract(itor++).value()));
    }
    m_timers.erase(m_timers.begin(), it);
    return expired;
}

void TimerManager::Reset(vector<TimerNode>& expired, Timestamp now) {

    for (auto& timer : expired) {
        if (timer->Repeat()) {
            LOG_DEBUG << "Is Repeat";
            timer->Restart(now);
            Insert(move(timer));
        }
    }

    Timestamp nextFinish;
    if (!m_timers.empty()) {
        nextFinish = (*m_timers.begin())->finishTime();
    }
    if (nextFinish.valid()) {
        StartTimer(m_timerfd, nextFinish);
    }
}

bool TimerManager::Insert(unique_ptr<Timer> ptimer) {
    bool isEarliest = false;
    Timestamp finishTime = ptimer->finishTime();
    auto it = m_timers.begin();
    if (it == m_timers.end() || finishTime < (*it)->finishTime()) {
        isEarliest = true;
    }
    m_timers.emplace(move(ptimer));
    return isEarliest;
}