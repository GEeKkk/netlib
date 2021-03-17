#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <map>

#include "netlib/base/Timestamp.h"
#include "EventLoop.h"

// using namespace muduo;

struct pollfd;
class Channel;

class Poller : noncopyable
{
public:
    using ChanVec = std::vector<Channel*>;
public:
    Poller(EventLoop* loop);
    ~Poller();

    muduo::Timestamp Poll(int TimeoutMs, ChanVec& ActiveChannels);

    void UpdateChannel(Channel* channel);

    void CheckInLoopThread();
    
private:
    void AddToActiveChannels(int kEvents, ChanVec& ActiveChannels) const; 
private:
    using PollFdVec = std::vector<struct pollfd>;
    using FdChanMap = std::map<int, Channel*>;

private:
    EventLoop* m_ownerloop;
    PollFdVec m_pollfds;
    FdChanMap m_chanMap;
};

#endif // POLLER_H