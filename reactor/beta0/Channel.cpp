#include "Channel.h"
#include "EventLoop.h"
#include "netlib/base/Logging.h"

#include <poll.h>

const int Channel::kNone = 0;
const int Channel::kRead = POLLIN | POLLPRI;
const int Channel::kWrite = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) 
    : m_eloop(loop),
      m_fd(fd),
      m_events(0),
      m_revents(0),
      m_index(-1)
    {
    }

Channel::~Channel() {
    
}

void Channel::SetRead(const EventCallback& cb) {
    m_ReadCallback = cb;
}
void Channel::SetWrite(const EventCallback& cb) {
    m_WriteCallback = cb;
}
void Channel::SetError(const EventCallback& cb) {
    m_ErrorCallback = cb;
}


void Channel::EnableRead() {
    m_events |= kRead;
    Register();
}

void Channel::Register() {
    m_eloop->UpdateChannel(this);
}

int Channel::fd() const {
    return m_fd;
}

int Channel::events() const {
    return m_events;
}

void Channel::set_revents(int r) {
    m_revents = r;
}

bool Channel::IsNone() const {
    return m_events == kNone;
}

int Channel::index() {
    return m_index;
}

void Channel::set_index(int idx) {
    m_index = idx;
}

void Channel::HandleEvent() {
    if (m_revents & POLLNVAL) {
        // LOG_WARN << "POLLNVAL";
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
    if (m_revents & (POLLERR | POLLNVAL)) {
        if (m_ErrorCallback) {
            m_ErrorCallback();
        }
    }
}