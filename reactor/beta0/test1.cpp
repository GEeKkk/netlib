#include "EventLoop.h"
#include <thread>

EventLoop *g_looper = nullptr;

void TFunc()
{
    g_looper->Loop();
}

int main()
{
    EventLoop looper;
    g_looper = &looper;
    std::thread t(TFunc);
    t.join();
}