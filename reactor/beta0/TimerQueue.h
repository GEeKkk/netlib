#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <set>
#include <vector>

#include "netlib/base/noncopyable.h"
#include "netlib/base/Timestamp.h"

#include "Callbacks.h"
#include "Channel.h"

using namespace muduo;

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : noncopyable
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId AddTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);
private:
    using Entry = std::pair<Timestamp, Timer*>;

    void HandleRead();
    std::vector<Entry> GetExpired(Timestamp now);
    void Reset(const std::vector<Entry>& expired, Timestamp now);
    bool Insert(Timer* timer);
private:
    using TimerSet = std::set<Entry>;

    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerfdChan;
    TimerSet m_timerSet;
};

#endif // TIMERQUEUE_H