#ifndef TIMERID_H
#define TIMERID_H

class Timer;

class TimerId
{
public:
    explicit TimerId(Timer* timer)
        : m_value(timer)
    {
    }
private:
    Timer* m_value;
};

#endif // TIMERID_H