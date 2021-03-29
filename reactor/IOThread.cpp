#include "IOThread.h"
#include "EventLoop.h"

#include <functional>

using namespace std;

IOThread::IOThread() 
    : m_loop(nullptr),
      m_isExit(false)
      {

      }

IOThread::~IOThread() {
    m_isExit = true;
    m_loop->Quit();
    m_iothread.join();
}

EventLoop* IOThread::StartLoop() {
    m_iothread = thread(&IOThread::ThreadFunc, this);

    {
        unique_lock<mutex> ulock(m_mu);
        while (m_loop == nullptr) {
            m_cond.wait(ulock);
        }
    }

    return m_loop;
}

void IOThread::ThreadFunc() {
    EventLoop loop;

    {
        unique_lock<mutex> ulock(m_mu);
        m_loop = &loop;
        m_cond.notify_one();
    }

    loop.Loop();
}