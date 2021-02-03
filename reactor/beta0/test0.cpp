#include "EventLoop.h"
#include "netlib/base/CurrentThread.h"
#include <stdio.h>
#include <thread>

void threadFunc()
{
    printf("threadFunc(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());

    EventLoop loop;
    loop.Loop();
}

int main()
{
    printf("main(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());

    EventLoop loop;

    std::thread th(threadFunc);
    loop.Loop();
    th.join();
    pthread_exit(NULL);
}
