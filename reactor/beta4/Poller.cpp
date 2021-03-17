#include "Poller.h"

#include "Channel.h"
#include "netlib/base/Logging.h"

#include <poll.h>

using namespace muduo;

Poller::Poller(EventLoop* loop)
    : m_ownerloop(loop)
{
}

Poller::~Poller() { }

Timestamp Poller::Poll(int TimeoutMs, ChanVec& ActiveChannels) {
    int kEvents = ::poll(m_pollfds.data(), m_pollfds.size(), TimeoutMs);
    Timestamp now(Timestamp::now());

    if (kEvents > 0) {
        LOG_DEBUG << kEvents << " events occurred";
        AddToActiveChannels(kEvents, ActiveChannels);
    } else if (kEvents == 0) {
        LOG_DEBUG << "Nothing happended";
    } else {
        LOG_ERROR << "Poller::Poll";
    }
    
    return now;
}

void Poller::AddToActiveChannels(int kEvents, ChanVec& ActiveChannels) const {
    for (auto it = m_pollfds.cbegin(); 
        it != m_pollfds.cend() && kEvents > 0; 
        ++it)
    {
        if (it->revents > 0) {
            --kEvents;
            auto kv = std::as_const(m_chanMap).find(it->fd);
            Channel* chan = kv->second;
            chan->SetRevents(it->revents);
            ActiveChannels.emplace_back(chan);
        }
    }
}


void Poller::CheckInLoopThread() {
    m_ownerloop->CheckInLoopThread();
}

void Poller::UpdateChannel(Channel* channel) {
    CheckInLoopThread();
    LOG_DEBUG << "fd = " << channel->GetFd() << " events = " << channel->GetEvents();

    if (channel->GetIndex() < 0) {
        struct pollfd pfd;
        pfd.fd = channel->GetFd();
        pfd.events = static_cast<short>(channel->GetEvents());
        pfd.revents = 0;
        m_pollfds.emplace_back(pfd);

        int idx = static_cast<int>(m_pollfds.size()) - 1;
        channel->SetIndex(idx);
        m_chanMap[pfd.fd] = channel;
    } else {
        int idx = channel->GetIndex();
        struct pollfd& pfd = m_pollfds[idx];
        pfd.events = static_cast<short>(channel->GetEvents());
        pfd.revents = 0;
        if (channel->IsNoneEvent()) {
            pfd.fd = -1;
        }
    }
}