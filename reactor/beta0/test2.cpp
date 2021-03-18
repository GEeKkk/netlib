#include "Channel.h"
#include "EventLoop.h"
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop = nullptr;

void Timeout() {
    printf("Timeout!\n");
    g_loop->Quit();
}

int main(int argc, char const *argv[])
{
    EventLoop loop;

    g_loop = &loop;

    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.SetRead(Timeout);
    channel.EnableRead();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.Loop();

    close(timerfd);
    return 0;
}
