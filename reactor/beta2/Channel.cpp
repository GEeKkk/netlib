#include "Channel.h"
#include "EventLoop.h"
#include "netlib/base/Logging.h"

#include <sstream>

#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) 
    : m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1)
{
}

void Channel::Update() {
    m_loop->UpdateChannel(this);
}

void Channel::HandleEvent() {
    if (m_revents & POLLNVAL) {
        LOG_WARN << "POLLNVAL";
    }

    if (m_revents & (POLLERR | POLLNVAL)) {
        if (m_ErrorCallback) {
            m_ErrorCallback();
        }
    }

    if (m_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (m_ReadCallback) {
            m_ReadCallback();
        }
    }

    if (m_revents & POLLOUT) {
        if (m_WriteCallback) {
            m_WriteCallback();
        }
    }
}