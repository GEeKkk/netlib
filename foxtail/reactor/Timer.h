#ifndef TIMER_H
#define TIMER_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"

#include "Callbacks.h"
#include <atomic>

class Timer : noncopyable
{
public:
    Timer(const TimerCallback& cb, 
          muduo::Timestamp when, 
          double intetval,
          size_t index);
    ~Timer() {
        printf("id = %ld dtor\n", m_id);
    }

    void Restart(muduo::Timestamp now);

    void Run() {
        m_callback();
    }

    muduo::Timestamp endTime() const {
        return m_endTime;
    }

    bool Repeat() const {
        return m_repeat;
    }

    size_t index() const {
        return m_index;
    }

    void SetIndex(size_t index) {
        m_index = index;
    }

    TimerId id() const {
        return m_id;
    }
private:
    const TimerCallback m_callback;
    muduo::Timestamp m_endTime;
    const double m_interval;
    const bool m_repeat;
    size_t m_index;
    const int64_t m_id;
    
    static std::atomic<int64_t> m_counter;
};

#endif // TIMER_H