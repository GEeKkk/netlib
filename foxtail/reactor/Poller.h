#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <unordered_map>

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"

class EventLoop;
class Channel;
struct pollfd;

class Poller : noncopyable
{
public:
    Poller(EventLoop* loop);

    muduo::Timestamp Poll(int timeoutMs, std::vector<Channel*>& activeChans);
    void FillActiveChannels(int numEvents, std::vector<Channel*>& activeChans);
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);
private:
    EventLoop* m_loop;
    std::vector<struct pollfd> m_pollfds;
    std::unordered_map<int, Channel*> m_chanMap;
};

#endif // POLLER_H