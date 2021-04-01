#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"

#include "foxtail/base/Logging.h"

#include <poll.h>
using namespace muduo;
using namespace std;

Poller::Poller(EventLoop* loop) 
    : m_loop(loop)
    {
    }


Timestamp Poller::Poll(int timeoutMs, vector<Channel*>& activeChans) {
    int num = poll(m_pollfds.data(), m_pollfds.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (num > 0) {
        FillActiveChannels(num, activeChans);
    } else if (num == 0) {
        // LOG_DEBUG << "Nothing happended";
    } else {
        LOG_SYSERR << "Poll";
    }
    return now;
}

void Poller::FillActiveChannels(int num, vector<Channel*>& activeChans) {
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
            pfd.fd = -channel->fd() - 1;
        }
    }
}


void Poller::RemoveChannel(Channel* channel) {
    int idx = channel->index();
    m_chanMap.erase(channel->fd());
    
    if (implicit_cast<size_t>(idx) == m_pollfds.size() - 1) {
        m_pollfds.pop_back();
    } else {
        int endfd = m_pollfds.back().fd;
        swap(m_pollfds[idx], m_pollfds.back());
        if (endfd < 0) {
            endfd = -endfd - 1;
        }
        m_chanMap[endfd]->set_index(idx);
        m_pollfds.pop_back();
    }
}
