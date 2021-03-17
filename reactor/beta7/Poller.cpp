#include "Poller.h"

#include "Channel.h"
#include "netlib/base/Logging.h"
// #include "netlib/base/Common.h"

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
            pfd.fd = -channel->GetFd() - 1;
        }
    }
}

void Poller::RemoveChannel(Channel* chan) {
    CheckInLoopThread();
    LOG_DEBUG << "fd = " << chan->GetFd();

    int idx = chan->GetIndex();

    size_t n = m_chanMap.erase(chan->GetFd());

    if (implicit_cast<size_t>(idx) == m_pollfds.size() - 1) {
        m_pollfds.pop_back();
    } else {
        int fd = m_pollfds.back().fd;
        std::iter_swap(m_pollfds.begin() + idx, m_pollfds.end() - 1);
        if (fd < 0) {
            fd = -fd - 1;
        }
        m_chanMap[fd]->SetIndex(idx);
        m_pollfds.pop_back();
    }
}