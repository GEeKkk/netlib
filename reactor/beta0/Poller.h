#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <vector>

#include "EventLoop.h"
#include "netlib/base/Timestamp.h"

struct pollfd;
class Channel;

using namespace muduo;

class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop* loop);
    ~Poller();

    Timestamp Poll(int timeoutMs, ChannelList* activeChans);

    void UpdateChannel(Channel* chan);

    void AssertInLoopThread() {
        m_OwnerLoop->AssertInLoopThread();
    }

private:
    void FillActiveChannels(int kEvents,
                            ChannelList* activeChan) const;
private:
    using PollFdList = std::vector<struct pollfd>;
    using ChanMap = std::map<int, Channel*>;

    EventLoop* m_OwnerLoop;
    PollFdList m_Pollfds;
    ChanMap m_Channels;

};


#endif // POLLER_H