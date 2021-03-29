#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"
#include "Channel.h"
#include "Callbacks.h"
#include "Timer.h"

#include <set>
#include <vector>

class EventLoop;

class TimerManager : noncopyable
{
public:
    TimerManager(EventLoop* loop);
    ~TimerManager();

    void AddTimer(const TimerCallback& cb, 
                  muduo::Timestamp when,
                  double interval);
private:
    using TimerNode = std::unique_ptr<Timer>;
    struct TimerCmp {
        using is_transparent = void;
        bool operator()(const TimerNode& lh, const TimerNode& rh) const {
            return lh->finishTime() < rh->finishTime();
        }
    };
    using Timers = std::set<TimerNode, TimerCmp>;
    using DoneTimers = std::vector<TimerNode>;
private:
    void HandleRead();
    DoneTimers GetExpiredTimers(muduo::Timestamp now);
    void Reset(DoneTimers& expired, muduo::Timestamp now);
    bool Insert(std::unique_ptr<Timer> ptimer);
private:
    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerChan;
    Timers m_timers;
};

#endif // TIMERMANAGER_H