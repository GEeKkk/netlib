#include <foxtail/reactor/EventLoop.h>
#include <foxtail/base/CurrentThread.h>
#include <stdio.h>
#include <thread>

using namespace std;
using namespace muduo;

EventLoop *g_loop;
int g_flag = 0;

void Test() {
    printf("shit\n");
}

void Print() {
    // g_loop->RunAfter(2.0, Test);
}

int main()
{
    printf("[main] pid = %d, flag = %d\n", getpid(), g_flag);

    EventLoop loop;
    g_loop = &loop;

    // loop.RunAfter(2, Print);
    thread th(Print);

    loop.Loop();
    th.join();
}
