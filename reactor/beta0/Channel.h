#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include "netlib/base/noncopyable.h"

// 前置声明
class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;

    Channel(EventLoop* loop, int fd);

    void HandleEvent();
    
    void SetReadCallback(const EventCallback& cb) {
        m_ReadCallback = cb;
    }

    void SetWriteCallback(const EventCallback& cb) {
        m_WriteCallback = cb;
    }

    void SetErrorCallback(const EventCallback& cb) {
        m_ErrorCallback = cb;
    }

    int GetFd() const {
        return m_fd;
    }

    int GetEvents() const {
        return m_events;
    }

    void SetRevents(int revt) {
        m_revents = revt;
    }

    bool IsNoneEvent() const {
        return m_events == kNoneEvent;
    }

    void EnableReading() {
        m_events |= kReadEvent;
        Update();
    }

    int GetIndex() {
        return m_index;
    }
    
    void SetIndex(int idx) {
        m_index = idx;
    }

    EventLoop* GetOwnerLoop() {
        return m_loop;
    }

private:
    void Update();
private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* m_loop;
    const int m_fd;
    int m_events;
    int m_revents;
    int m_index;

    EventCallback m_ReadCallback;
    EventCallback m_WriteCallback;
    EventCallback m_ErrorCallback;
};

#endif // CHANNEL_H