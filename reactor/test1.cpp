#include "EventLoop.h"
#include <thread>

EventLoop *g_loop = nullptr;

void threadFunc()
{
    g_loop->Loop();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    std::thread th(threadFunc);
    th.join();
}
