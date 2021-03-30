#include <foxtail/reactor/Channel.h>
#include <foxtail/reactor/EventLoop.h>
#include <foxtail/base/Timestamp.h>

#include <sys/timerfd.h>
#include <string.h>

using namespace std;
using namespace muduo;

EventLoop* g_loop = nullptr;

void Timeout(Timestamp time) {
    printf("Timeout!\n");
    g_loop->Quit();
}

int CreateTimerFd() {
    return timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
}

void CountDown(int timerfd) {
    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    timerfd_settime(timerfd, 0, &howlong, NULL);
}

int main(int argc, char const *argv[])
{
    EventLoop loop;
    g_loop = &loop;

    int timerfd = CreateTimerFd();

    Channel chan(g_loop, timerfd);
    chan.SetRead(Timeout);
    chan.EnableRead();

    CountDown(timerfd);

    loop.Loop();

    close(timerfd);

    return 0;
}
