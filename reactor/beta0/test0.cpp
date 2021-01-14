#include "EventLoop.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>

void TFunc() {
    printf("TFunc: pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());
    EventLoop looper;
    looper.Loop();
}

int main(int argc, char const *argv[])
{
    printf("main: pid=%d, tid=%d\n", getpid(), CurrentThread::tid());

    EventLoop mainloop;
    std::thread th(TFunc);
    mainloop.Loop();

    th.join();
    pthread_exit(NULL);
    return 0;
}
