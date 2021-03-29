#ifndef CHANNEL_H
#define CHANNEL_H

#include "netlib/base/noncopyable.h"
#include "netlib/base/Timestamp.h"
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

    void SetRead(const ReadEventCallback& cb);
    void SetWrite(const EventCallback& cb);
    void SetError(const EventCallback& cb);
    void SetClose(const EventCallback& cb);

    void EnableRead();
    void EnableWrite();
    void DisableWrite();
    void DisableAll();

    bool isWriting() const {
        return m_events & kWrite;
    }

    int fd() const;
    int events() const;
    void set_revents(int revts);

    int index();
    void set_index(int idx);

    bool IsNone() const;



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