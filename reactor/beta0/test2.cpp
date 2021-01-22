#include "Channel.h"
#include "EventLoop.h"

#include <stdio.h>
#include <strings.h>
#include <sys/timerfd.h>
#include <unistd.h>

EventLoop* g_loop = nullptr;

void Timeout() {
    printf("Timeout!!!\n");
    g_loop->Quit();
}

int main(int argc, char const *argv[])
{
    EventLoop mLoop;
    g_loop = &mLoop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel chan(&mLoop, timerfd);
    chan.SetReadCallback(Timeout);
    chan.EnableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    mLoop.Loop();

    ::close(timerfd);

    return 0;
}
