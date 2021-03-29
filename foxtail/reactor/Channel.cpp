#include "Channel.h"
#include "EventLoop.h"
#include "foxtail/base/Logging.h"


#include <poll.h>

const int Channel::kNone = 0;
const int Channel::kRead = POLLIN | POLLPRI;
const int Channel::kWrite = POLLOUT;

using namespace muduo;

Channel::Channel(EventLoop* loop, int fd) 
    : m_eloop(loop),
      m_fd(fd),
      m_events(0),
      m_revents(0),
      m_index(-1),
      m_eventHandling(false)
    {
    }

Channel::~Channel() {
    
}

void Channel::DisableAll() {
    m_events = kNone;
    Register();
}

void Channel::EnableRead() {
    m_events |= kRead;
    Register();
}

void Channel::EnableWrite() {
    m_events |= kWrite;
    Register();
}

void Channel::DisableWrite() {
    m_events &= ~kWrite;
    Register();
}

void Channel::Register() {
    m_eloop->UpdateChannel(this);
}

void Channel::HandleEvent(Timestamp recvTime) {
    m_eventHandling = true;
    if (m_revents & POLLNVAL) {
        LOG_WARN << "POLLNVAL";
    }
    if (m_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (m_ReadCallback) {
            m_ReadCallback(recvTime);
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
    if ((m_revents & POLLHUP) && !(m_revents & POLLIN)) {
        if (m_CloseCallback) {
            m_CloseCallback();
        }
    }
    m_eventHandling = false;
}