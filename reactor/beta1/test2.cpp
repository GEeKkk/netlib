#include "Channel.h"
#include "EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop = nullptr;
int g_fdA = 0;
int g_fdB = 0;
void TestChanA()
{
    printf("This Channel A\n");
    long int timersElapsed = 0;
    (void) read(g_fdA, &timersElapsed, 8);
    // g_loop->Quit();
}

void TestChanB() {
    printf("This Chan B\n");
    long int tmp = 0;
    read(g_fdB, &tmp, 9);
}

int main(int argc, char const *argv[])
{
    EventLoop loop;
    g_loop = &loop;

    int timerfdA = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    g_fdA = timerfdA;
    Channel chanA(&loop, timerfdA);
    chanA.SetReadCallback(TestChanA);
    chanA.EnableRead();

    struct itimerspec durationA;
    bzero(&durationA, sizeof(durationA));
    durationA.it_value.tv_sec = 30;
    durationA.it_interval.tv_sec = 5;
    ::timerfd_settime(timerfdA, 0, &durationA, NULL);

    // int timerfdB = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    // g_fdB = timerfdB;
    // Channel chanB(&loop, timerfdB);
    // chanB.SetReadCallback(TestChanB);
    // chanB.EnableRead();

    // struct itimerspec durationB;
    // bzero(&durationB, sizeof(durationB));
    // durationB.it_value.tv_sec = 40;
    // durationB.it_interval.tv_sec = 6;
    // ::timerfd_settime(timerfdB, 0, &durationB, NULL);


    loop.Loop();

    ::close(timerfdA);
    // ::close(timerfdB);
    return 0;
}
