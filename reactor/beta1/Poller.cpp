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
    for (const auto& it : m_pollfds) {
        if (it.revents > 0) {
            --kEvents;
            auto kv = std::as_const(m_chanMap).find(it.fd);
            Channel* chan = kv->second;
            chan->SetRevents(it.revents);
            ActiveChannels.emplace_back(chan);
        }
        if (kEvents <= 0) {
            break;
        }
    }
}


void Poller::CheckInLoopThread() {
    m_ownerloop->CheckInLoopThread();
}

void Poller::UpdateChannel(Channel* chan) {
    CheckInLoopThread();
    LOG_DEBUG << "fd = " << chan->fd() << " events = " << chan->events();
    if (chan->index() < 0) {
        struct pollfd pfd;
        pfd.fd = chan->fd();
        pfd.events = static_cast<short>(chan->events());
        pfd.revents = 0;
        m_pollfds.emplace_back(pfd);
        int idx = static_cast<int>(m_pollfds.size()) - 1;
        chan->SetIndex(idx);
        m_chanMap[pfd.fd] = chan;
    } else {
        int idx = chan->index();
        struct pollfd& pfd = m_pollfds[idx];
        pfd.events = static_cast<short>(chan->events());
        pfd.revents = 0;
        if (chan->IsNoneEvent()) {
            pfd.fd = -1;
        }
    }
}