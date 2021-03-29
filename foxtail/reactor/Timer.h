#ifndef TIMER_H
#define TIMER_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"

#include "Callbacks.h"

class Timer : noncopyable
{
public:
    Timer(const TimerCallback& cb, muduo::Timestamp when, double intv);

    void Run();

    muduo::Timestamp finishTime() const;
    bool Repeat() const;
    void Restart(muduo::Timestamp now);

private:
    const TimerCallback m_callback;
    muduo::Timestamp m_finishTime;
    const double m_interval;
    const bool m_repeat;
};

#endif // TIMER_H