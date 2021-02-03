#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <vector>

#include "EventLoop.h"
#include "netlib/base/Timestamp.h"

// 前置声明
struct pollfd;
class Channel;

using namespace muduo;
// IO 多路复用 poll(2)
// Poller类不拥有Channel对象
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop* loop);
    ~Poller();
    // 轮询I/O事件
    // 必须在loop线程里调用
    Timestamp Poll(int timeoutMs, ChannelList* activeChans);

    // 更新 I/O 事件
    // 必须在loop线程里调用
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

    EventLoop* m_OwnerLoop; // 
    PollFdList m_Pollfds; // 轮询fd列表(vector) 
    ChanMap m_Channels; // (fd, Channel) 键值对
};


#endif // POLLER_H