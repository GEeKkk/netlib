#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <map>

#include "netlib/base/Timestamp.h"
#include "EventLoop.h"

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
    EventLoop* m_ownerloop;
    std::vector<struct pollfd> m_pollfds;
    std::map<int, Channel*> m_chanMap;
};

#endif // POLLER_H