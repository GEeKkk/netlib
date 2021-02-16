#include "EventLoop.h"

#include "Channel.h"
#include "Poller.h"

#include "netlib/base/CurrentThread.h"
#include "netlib/base/Logging.h"

#include <sys/eventfd.h>
#include <signal.h>

using namespace muduo;

__thread EventLoop* t_loopInCurrentThread = nullptr;
const int kPollTimeMs = 10000;

static int CreateEventFd() {
    int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evfd < 0) {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }
    return evfd;
}

class IgnoreSigPipe
{
public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe initObj;

EventLoop::EventLoop()
        : m_looping(false),
          m_quit(false),
          m_callPendingFuncs(false),
          m_threadId(CurrentThread::tid()),
          m_poller(std::make_unique<Poller>(this)),
          m_wakeupFd(CreateEventFd()),
          m_wakeupChan(std::make_unique<Channel>(this, m_wakeupFd))
{
    LOG_DEBUG << "EventLoop created [" << this << "] in thread (" << m_threadId << ")";
    /// 每个线程只能有一个eventloop, 
    /// 创建之前需要检查是否已经创建了其他loop对象
    if (t_loopInCurrentThread) {
        LOG_FATAL << "Another EventLoop [" << t_loopInCurrentThread 
                  << "] exists in this thread (" << m_threadId << ")";
    } else {
        t_loopInCurrentThread = this;
    }

    m_wakeupChan->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
    m_wakeupChan->EnableRead();
}

EventLoop::~EventLoop() {
    ::close(m_wakeupFd);
    t_loopInCurrentThread = nullptr;
}

void EventLoop::Loop() {
    // 事件循环必须在IO线程中执行，pre-condition
    CheckInLoopThread();
    m_looping = true;
    m_quit = false;

    while (!m_quit) {
        m_ActiveChannels.clear();
        m_pollRetTime = m_poller->Poll(kPollTimeMs, m_ActiveChannels);

        for (auto& it : m_ActiveChannels) {
            it->HandleEvent(m_pollRetTime);
        }

        DoPendingFuncs();
    }


    LOG_DEBUG << "EventLoop [" << this << "] stop looping.";
    m_looping = false;
}

void EventLoop::Quit() {
    m_quit = true;
    if (!IsInLoopThread()) {
        WakeUp();
    }
}

void EventLoop::UpdateChannel(Channel* channel) {
    CheckInLoopThread();
    m_poller->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
    CheckInLoopThread();
    m_poller->RemoveChannel(channel);
}


void EventLoop::CheckInLoopThread() {
     if (!IsInLoopThread()) {
         AbortNotInLoopThread();
     }
}

bool EventLoop::IsInLoopThread() const {
    return m_threadId == CurrentThread::tid();
}

void EventLoop::AbortNotInLoopThread() {
    LOG_FATAL << "EventLoop Abort. Created in thread(" << m_threadId 
              << "), but current thread is (" << CurrentThread::tid() << ")";
}

void EventLoop::queueInLoop(const Functor& func) {
    {
        std::lock_guard<std::mutex> lockgurd(m_mutex);
        m_pendingFuncs.emplace_back(func);
    }

    if (!IsInLoopThread() || m_callPendingFuncs) {
        WakeUp();
    }
}

void EventLoop::RunInLoop(const Functor& func) {
    if (IsInLoopThread()) {
        func();
    } else {
        queueInLoop(func);
    }
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::WakeUp writes " << n << "bytes rather than 8";
    }
}


void EventLoop::DoPendingFuncs() {
    std::vector<Functor> vFuncs;
    m_callPendingFuncs = true;

    {
        std::lock_guard<std::mutex> lockguard(m_mutex);
        vFuncs.swap(m_pendingFuncs);
    }

    for (auto& fn : vFuncs) {
        fn();
    }

    m_callPendingFuncs = false;
}

void EventLoop::HandleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::HandleRead reads " << n << " bytes rather than 8";
    }
}