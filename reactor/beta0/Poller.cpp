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
    for (auto it = m_Pollfds.begin();
         it != m_Pollfds.end() && kEvents > 0;
         ++it)
    {
        if (it->revents > 0) {
            --kEvents;
            auto chanv = m_Channels.find(it->fd);
            auto chan = chanv->second;

            chan->SetRevents(it->revents);
            activeChans->emplace_back(chan);
        }
    }
}

void Poller::UpdateChannel(Channel* chan)
{
    AssertInLoopThread();
    LOG_TRACE << "fd = " << chan->GetFd() << " events = " << chan->GetEvents();
    if (chan->GetIndex() < 0) {
        struct pollfd pfd;
        pfd.fd = chan->GetFd();
        pfd.events = static_cast<short>(chan->GetEvents());
        pfd.revents = 0;
        m_Pollfds.emplace_back(pfd);
        int tmpidx = static_cast<int>(m_Pollfds.size()) - 1;
        chan->SetIndex(tmpidx);
        m_Channels[pfd.fd] = chan;
    } else {
        int idx = chan->GetIndex();
        struct pollfd& pfd = m_Pollfds[idx];
        pfd.events = static_cast<short>(chan->GetEvents());
        pfd.revents = 0;
        if (chan->IsNoneEvent()) {
            pfd.fd = -1;
        }
    }
}