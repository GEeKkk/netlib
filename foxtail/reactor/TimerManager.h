#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"
#include "Channel.h"
#include "Callbacks.h"
#include "Timer.h"

#include <vector>
#include <unordered_map>

class EventLoop;

class TimerQueue : noncopyable
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId Add(const TimerCallback& cb, 
                  muduo::Timestamp when,
                  double interval);
    
    void Stop(TimerId id);
private:
    using TimerNode = std::shared_ptr<Timer>;
    using Timers = std::vector<TimerNode>;
    using Map = std::unordered_map<TimerId, std::weak_ptr<Timer>>;
private:
    void AddInLoop(TimerNode& timer);
    void StopInLoop();
    void HandleRead();
    Timers expired(muduo::Timestamp now);
    void reset(Timers& expired, muduo::Timestamp now);
    void up(size_t index);
    void down(size_t index);
    void Swap(size_t index1, size_t index2);
    bool insert(TimerNode& ptimer);
    void RemoveFromHeap(size_t index);
    void RemoveFromMap(TimerId id);
private:
    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerChan;
    Timers m_heap;
    Map m_idMap;
};

#endif // TIMERMANAGER_H