#include "TimerManager.h"
#include "EventLoop.h"

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

TimerQueue::TimerQueue(EventLoop* loop)
    : m_loop(loop),
      m_timerfd(CreateTimerfd()),
      m_timerChan(loop, m_timerfd)
    {
        m_timerChan.SetRead(bind(&TimerQueue::HandleRead, this));
        m_timerChan.EnableRead();
    }

TimerQueue::~TimerQueue() {
    close(m_timerfd);
}

TimerId TimerQueue::Add(const TimerCallback& cb,
                    Timestamp when,
                    double interval)
{
    m_heap.reserve(m_heap.size() + 1);
    auto timer = make_shared<Timer>(cb, when, interval, m_heap.size());
    auto id = timer->id();
    bool isEarliest = insert(timer);
    if (isEarliest) {
        StartTimer(m_timerfd, when);
    }
    return id;
}

void TimerQueue::Stop(TimerId id) {
    auto it = m_idMap.find(id);
    if (it != m_idMap.end()) {
        if (!it->second.expired()) {
            RemoveFromHeap(it->second.lock()->index());
            RemoveFromMap(id);
        }
    }
}

void TimerQueue::HandleRead() {
    ReadTimerfd(m_timerfd);
    Timestamp now = Timestamp::now();
    auto ready = expired(now);
    for (auto& timer : ready) {
        timer->Run();
    }
    reset(ready, now);
}

vector<shared_ptr<Timer>> TimerQueue::expired(Timestamp now) {
    vector<shared_ptr<Timer>> expired;
    while (!m_heap.empty() && m_heap[0]->endTime() < now) {
        size_t index = m_heap[0]->index();
        expired.emplace_back(m_heap[0]);
        RemoveFromHeap(index);
    }
    return expired;
}

void TimerQueue::reset(vector<TimerNode>& expired, Timestamp now) {
    for (auto& timer : expired) {
        if (timer->Repeat()) {
            timer->Restart(now);
            insert(timer);
        } else {
            m_idMap.erase(timer->id());
        }
    }

    Timestamp nextFinish;
    if (!m_heap.empty()) {
        nextFinish = m_heap[0]->endTime();
    }
    if (nextFinish.valid()) {
        StartTimer(m_timerfd, nextFinish);
    }
}

void TimerQueue::up(size_t index) {
    size_t parent = (index - 1) / 2;
    while (index > 0 && m_heap[index]->endTime() < m_heap[parent]->endTime()) {
        Swap(index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void TimerQueue::down(size_t index) {
    size_t child = index * 2 + 1;
    while (child < m_heap.size()) {
        size_t min_child = 0;
        if (child + 1 == m_heap.size() || 
            m_heap[child]->endTime() < m_heap[child+1]->endTime())
        {
            min_child = child;
        } else {
            min_child = child + 1;
        }

        if (m_heap[index]->endTime() < m_heap[min_child]->endTime()) {
            break;
        }
        Swap(index, min_child);
        index = min_child;
        child = index * 2 + 1;
    }
}

void TimerQueue::Swap(size_t index1, size_t index2) {
    swap(m_heap[index1], m_heap[index2]);
    m_heap[index1]->SetIndex(index1);
    m_heap[index2]->SetIndex(index2);
}

bool TimerQueue::insert(shared_ptr<Timer>& timer) {
    bool isEarliest = false;
    Timestamp endtime = timer->endTime();
    if (m_heap.empty() || endtime < m_heap[0]->endTime()) {
        isEarliest = true;
    }
    m_idMap[timer->id()] = timer;
    m_heap.push_back(move(timer));
    up(m_heap.size() - 1);
    return isEarliest;
}

void TimerQueue::RemoveFromHeap(size_t index) {
    if (!m_heap.empty() && index < m_heap.size()) {
        if (index == m_heap.size() - 1) {
            m_heap.pop_back();
        } else {
            Swap(index, m_heap.size() - 1);
            m_heap.pop_back();
            size_t parent = (index - 1) / 2;
            if (index > 0 && m_heap[index]->endTime() < m_heap[parent]->endTime()) {
                up(index);
            } else {
                down(index);
            }
        }
    }
}

void TimerQueue::RemoveFromMap(TimerId id) {
    m_idMap.erase(id);
}