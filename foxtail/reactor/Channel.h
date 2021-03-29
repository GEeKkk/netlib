#ifndef CHANNEL_H
#define CHANNEL_H

#include "foxtail/base/noncopyable.h"
#include "foxtail/base/Timestamp.h"
#include <functional>

class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(muduo::Timestamp)>;
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void HandleEvent(muduo::Timestamp recvTime);

    void EnableRead();
    void EnableWrite();
    void DisableWrite();
    void DisableAll();

    void SetRead(const ReadEventCallback& cb) {
        m_ReadCallback = cb;
    }
    void SetWrite(const EventCallback& cb) {
        m_WriteCallback = cb;
    }
    void SetError(const EventCallback& cb) {
        m_ErrorCallback = cb;
    }

    void SetClose(const EventCallback& cb) {
        m_CloseCallback = cb;
    }

    bool isWriting() const {
        return m_events & kWrite;
    }

    int fd() const {
        return m_fd;
    }

    int events() const {
        return m_events;
    }

    void set_revents(int r) {
        m_revents = r;
    }

    bool IsNone() const {
        return m_events == kNone;
    }

    int index() {
        return m_index;
    }

    void set_index(int idx) {
        m_index = idx;
    }
private:
    void Register();
private:
    ReadEventCallback m_ReadCallback;
    EventCallback m_WriteCallback;
    EventCallback m_ErrorCallback;
    EventCallback m_CloseCallback;

    static const int kNone;
    static const int kRead;
    static const int kWrite;
private:
    EventLoop* m_eloop;
    const int m_fd;
    int m_events;
    int m_revents;
    int m_index;
    bool m_eventHandling;
};

#endif // CHANNEL_H