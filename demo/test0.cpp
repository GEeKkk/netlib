#include <foxtail/reactor/EventLoop.h>
#include <foxtail/base/CurrentThread.h>

#include <stdio.h>
#include <thread>

#include <functional>

// #include "bench.h"

using namespace std;

void Tester()
{
    printf("[sub] pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    EventLoop loop;
    loop.Loop();
}

int main()
{
    // Bench::Run(bind(SysCall), 1000000, FuncName(SysCall));
    // Bench::Run(bind(UserCall), 1000000, FuncName(UserCall));
    // Bench::Run(bind(Call), 1000000, FuncName(Call));
    printf("[main] pid = %d, tid = %d\n", getpid(), CurrentThread::tid());

    EventLoop loop;
    thread th(Tester);
    loop.Loop();
    th.join();
}
