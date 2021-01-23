#include "Timer.h"

void Timer::ReStart(Timestamp now) {
    if (m_repeat) {
        m_expiration = addTime(now, m_interval);
    } else {
        m_expiration = Timestamp::invalid();
    }
}