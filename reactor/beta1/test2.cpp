#include "Channel.h"
#include "EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop = nullptr;

void TestTimeout()
{
    printf("Timeout!!!\n");
    g_loop->Quit();
}

int main(int argc, char const *argv[])
{
    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel chan(&loop, timerfd);
    chan.SetReadCallback(TestTimeout);
    chan.EnableRead();

    struct itimerspec duration;
    bzero(&duration, sizeof(duration));
    duration.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &duration, NULL);
    loop.Loop();

    ::close(timerfd);
    return 0;
}
