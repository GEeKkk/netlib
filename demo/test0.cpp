#include <foxtail/reactor/EventLoop.h>
#include <foxtail/base/CurrentThread.h>

#include <stdio.h>
#include <thread>

using namespace std;

void Tester()
{
    printf("[sub] pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    EventLoop loop;
    loop.Loop();
}

int main()
{
    printf("[main] pid = %d,tid = %d\n", getpid(), CurrentThread::tid());

    EventLoop loop;
    thread th(Tester);
    loop.Loop();
    th.join();
}
