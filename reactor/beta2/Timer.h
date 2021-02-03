#ifndef TIMER_H
#define TIMER_H

#include "netlib/base/noncopyable.h"
#include "netlib/base/Timestamp.h"

#include "Callbacks.h"
// #include <functional>

using namespace muduo;

class Timer : noncopyable
{
public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
        : m_callback(cb),
          m_expiration(when),
          m_interval(interval),
          m_repeat(m_interval > 0.0)
    {
    }

    void ReStart(Timestamp now);

    void Run() const {
        m_callback();
    }

    Timestamp GetExpiration() const {
        return m_expiration;
    }

    bool IsRepeat() const {
        return m_repeat;
    }

private:
    const TimerCallback m_callback;
    Timestamp m_expiration; // 
    const double m_interval;
    const bool m_repeat;
};

#endif // TIMER_H