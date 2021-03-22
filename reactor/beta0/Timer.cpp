#include "Timer.h"
using namespace muduo;

Timer::Timer(const TimerCallback& cb, Timestamp when, double intv)
    : m_callback(cb),
      m_finishTime(when),
      m_interval(intv),
      m_repeat(intv > 0.0)
    {
    }

void Timer::Run() {
    m_callback();
}

Timestamp Timer::finishTime() const {
    return m_finishTime;
}

bool Timer::Repeat() const {
    return m_repeat;
}

void Timer::Restart(Timestamp now) {
    if (m_repeat) {
        m_finishTime = addTime(now, m_interval);
    } else {
        m_finishTime = Timestamp::invalid();
    }
} 