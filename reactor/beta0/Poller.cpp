#include "Poller.h"
#include "Channel.h"
#include "netlib/base/Logging.h"

#include <poll.h>

using namespace muduo;

Poller::Poller(EventLoop* loop)
    : m_OwnerLoop(loop)
{
}

Poller::~Poller() 
{
}

Timestamp Poller::Poll(int timeoutMs, ChannelList* activeChans)
{
    // 返回就绪事件
    int kEvents = ::poll(m_Pollfds.data(), m_Pollfds.size(), timeoutMs);
    Timestamp now(Timestamp::now());

    if (kEvents > 0) {
        LOG_TRACE << kEvents << " events happended";
        FillActiveChannels(kEvents, activeChans);
    } else if (kEvents == 0) {
        LOG_TRACE <<  " nothing happended";
    } else {
        LOG_SYSERR << "Poller::Poll()";
    }

    return now;
}

void Poller::FillActiveChannels(int kEvents, 
                                ChannelList* activeChans) const 
{
    // 遍历pollfd数组，找到就绪事件，并加入到活跃channel列表内
    for (auto it = m_Pollfds.cbegin();
         it != m_Pollfds.cend() && kEvents > 0;
         ++it)
    {
        if (it->revents > 0) {
            --kEvents; // 就绪事件--
            // 找到就绪描述符对应的Channel
            auto kvItor = std::as_const(m_Channels).find(it->fd);
            Channel* pChan = kvItor->second;
            pChan->SetRevents(it->revents);
            activeChans->emplace_back(pChan);
        }
    }
}

void Poller::UpdateChannel(Channel* chan)
{
    AssertInLoopThread();
    LOG_TRACE << "fd = " << chan->GetFd() << " events = " << chan->GetEvents();
    if (chan->GetIndex() < 0) {
        // 新fd，加入到PollFd里
        struct pollfd pfd;
        pfd.fd = chan->GetFd();
        pfd.events = static_cast<short>(chan->GetEvents());
        pfd.revents = 0;
        m_Pollfds.emplace_back(pfd);
        int tmpidx = static_cast<int>(m_Pollfds.size()) - 1;
        chan->SetIndex(tmpidx);
        m_Channels[pfd.fd] = chan;
    } else {
        // fd已存在，更新该fd上的事件
        int idx = chan->GetIndex();
        struct pollfd& pfd = m_Pollfds[idx];
        pfd.events = static_cast<short>(chan->GetEvents());
        pfd.revents = 0;
        if (chan->IsNoneEvent()) {
            pfd.fd = -1;
        }
    }
}