#include "EventLoop.h"
#include <functional>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
int cnt = 0;
EventLoop* g_loop = nullptr;

void PrintTid() {
    printf("pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());
    printf("now %s\n", Timestamp::now().toFormattedString().c_str());
}

void Print(const char* msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20) {
        g_loop->Quit();
    }
}

int main(int argc, char const *argv[])
{
    PrintTid();
    EventLoop mLoop;
    g_loop = &mLoop;
    Print("main");

    // mLoop.RunAfter(1, std::bind(Print, "once1"));
    // mLoop.RunAfter(1.5, std::bind(Print,"once1.5"));
    // mLoop.RunAfter(2.5, std::bind(Print, "once2.5"));
    mLoop.RunAfter(3.5, std::bind(Print, "once3.5"));
    // mLoop.RunEvery(2,  std::bind(Print, "every2"));
    mLoop.RunEvery(3,  std::bind(Print, "every3"));

    mLoop.Loop();
    Print("main loop exits");
    sleep(1);
    return 0;
}
