#include "Channel.h"
#include "EventLoop.h"

#include "netlib/base/Logging.h"

#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) 
    : m_Loop(loop),
      m_fd(fd),
      m_events(0),
      m_revents(0),
      m_index(-1)
{
}

void Channel::SetReadCallback(const EventCallback& cb) {
    m_ReadCallback = cb;
}

void Channel::SetWriteCallback(const EventCallback& cb) {
    m_WriteCallback = cb;
}

void Channel::SetErrorCallback(const EventCallback& cb) {
    m_ErrorCallback = cb;
}

int Channel::GetFd() const {
    return m_fd;
}

int Channel::GetEvents() const {
    return m_events;
}

void Channel::SetRevents(int revt) {
    m_revents = revt;
}

bool Channel::IsNoneEvent() const {
    return m_events == kNoneEvent;
}

void Channel::EnableRead() {
    m_events |= kReadEvent;
    Update();
}

void Channel::Update() {
    m_Loop->UpdateChannel(this);
}

int Channel::GetIndex() {
    return m_index;
}

void Channel::SetIndex(int idx) {
    m_index = idx;
}

EventLoop* Channel::OwnerLoop() {
    return m_Loop;
}


void Channel::HandleEvent() {
    if (m_revents & POLLNVAL) {
        LOG_WARN << "Channel::HandleEvent() POLLNVAL";
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