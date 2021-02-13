#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include "netlib/base/noncopyable.h"

class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
public:
    Channel(EventLoop* loop, int fd);
    void HandleEvent();
    void SetReadCallback(const EventCallback& cb);
    void SetWriteCallback(const EventCallback& cb);
    void SetErrorCallback(const EventCallback& cb);

    void EnableRead();

    int GetFd() const;
    int GetEvents() const;
    void SetRevents(int revt);

    bool IsNoneEvent() const;

    int GetIndex();
    void SetIndex(int idx);
    EventLoop* OwnerLoop();

private:
    void Update();

private:
    EventLoop* m_Loop;
    const int m_fd;

    int m_events;
    int m_revents;
    int m_index;

private:
    EventCallback m_ReadCallback;
    EventCallback m_WriteCallback;
    EventCallback m_ErrorCallback;

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};

#endif // CHANNEL_H