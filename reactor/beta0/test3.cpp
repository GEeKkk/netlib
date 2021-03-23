#include "EventLoop.h"
#include "netlib/base/CurrentThread.h"
#include <stdio.h>

using namespace std;
using namespace muduo;

EventLoop *g_loop;
int g_flag = 0;

void run4()
{
    printf("run4(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->Quit();
}

void run3()
{
    printf("run3(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->RunAfter(3, run4);
    g_flag = 3;
}

void run2()
{
    printf("run2(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->Stored(run3);
}

void run1()
{
    g_flag = 1;
    printf("run1(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->RunInLoop(run2);
    g_flag = 2;
}

int main()
{
    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);

    EventLoop loop;
    g_loop = &loop;

    loop.RunAfter(2, run1);
    loop.Loop();
    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
}
