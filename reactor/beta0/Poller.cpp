#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"

#include "netlib/base/Logging.h"

#include <poll.h>
using namespace muduo;

Poller::Poller(EventLoop* loop) 
    : m_eloop(loop)
    {
    }


Timestamp Poller::Poll(int timeoutMs, std::vector<Channel*>& activeChans) {
    int num = poll(m_pollfds.data(), m_pollfds.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (num > 0) {
        // LOG_DEBUG << num << " event happended";
        FillActiveChannels(num, activeChans);
    } else if (num == 0) {
        LOG_DEBUG << "Nothing happended";
    } else {
        LOG_SYSERR << "Poll";
    }
    return now;
}

void Poller::FillActiveChannels(int num, std::vector<Channel*>& activeChans) {
    for (auto& pfd : m_pollfds) {
        if (pfd.revents > 0) {
            --num;
            auto it = m_chanMap.find(pfd.fd);
            Channel* chan = it->second;
            chan->set_revents(pfd.revents);
            activeChans.emplace_back(chan);
        }
        if (num <= 0) {
            break;
        }
    }
}

void Poller::UpdateChannel(Channel* channel) {
    if (channel->index() < 0) {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        m_pollfds.emplace_back(pfd);
        int idx = static_cast<int>(m_pollfds.size()) - 1;
        channel->set_index(idx);
        m_chanMap[pfd.fd] = channel;
    } else {
        int idx = channel->index();
        auto& pfd = m_pollfds[idx];
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->IsNone()) {
            pfd.fd = -1;
        }
    }
}


void Poller::RemoveChannel(Channel* channel) {
    int idx = channel->index();
    m_chanMap.erase(channel->fd());
    m_pollfds.erase(m_pollfds.begin() + idx);
    // if (m_pollfds.size() - 1 == idx) {
    //     m_pollfds.pop_back();
    // } else {
    //     int fd = m_pollfds.back().fd;
        

    // }
}
