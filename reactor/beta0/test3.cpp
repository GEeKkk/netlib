#include "EventLoop.h"
#include "netlib/base/Timestamp.h"
#include "netlib/base/CurrentThread.h"
#include <stdio.h>
using namespace muduo;
int cnt = 0;
EventLoop *g_loop = nullptr;

void printTid()
{
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char *msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20)
    {
        g_loop->Quit();
    }
}

int main()
{
    printTid();
    EventLoop loop;
    g_loop = &loop;

    print("main");
    // loop.RunAfter(1, std::bind(print, "once1"));
    // loop.RunAfter(1.5, std::bind(print, "once1.5"));
    // loop.RunAfter(2.5, std::bind(print, "once2.5"));
    // loop.RunAfter(3.5, std::bind(print, "once3.5"));
    // loop.RunEvery(2, std::bind(print, "every2"));
    loop.RunEvery(3, std::bind(print, "every3"));

    loop.Loop();
    print("main loop exits");
    sleep(1);
}
