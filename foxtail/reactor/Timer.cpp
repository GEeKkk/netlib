#include "Timer.h"

using namespace muduo;
using namespace std;

atomic<int64_t> Timer::m_counter;

Timer::Timer(const TimerCallback& cb, 
            Timestamp when, 
            double interval,
            size_t index)
    : m_callback(cb),
      m_endTime(when),
      m_interval(interval),
      m_repeat(interval > 0.0),
      m_index(index),
      m_id(m_counter++)
    {
    }

void Timer::Restart(Timestamp now) {
    if (m_repeat) {
        m_endTime = addTime(now, m_interval);
    } else {
        m_endTime = Timestamp::invalid();
    }
} 