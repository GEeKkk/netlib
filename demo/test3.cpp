#include <foxtail/reactor/EventLoop.h>
#include <foxtail/base/CurrentThread.h>
#include <stdio.h>
#include <thread>
#include <iostream>

using namespace std;
using namespace muduo;

EventLoop *g_loop;
int g_flag = 0;

void Test() {
    printf("test after every 3 sec\n");
}

void Print(int sec) {
    // g_loop->RunAfter(2.0, Test);
    printf("print after %ds\n", sec);
}

int main()
{
    printf("[main] pid = %d, flag = %d\n", getpid(), g_flag);

    auto t = Timestamp::now();
    sleep(3);
    auto now = Timestamp::Since(t);
    
    cout << now.Seconds() << endl;
    cout << now.Milliseconds() << endl;
    // EventLoop loop;
    // g_loop = &loop;

    // loop.RunAfter(10.0, bind(Print, 10));
    // loop.RunAfter(2.0, bind(Print, 2));
    // loop.RunAfter(5.0, bind(Print, 5));
    // loop.RunAfter(3.0, bind(Print, 3));
    // loop.StopTimer(id);
    // loop.RunEvery(3, Test);
    // thread th(Print);

    // loop.Loop();
    // th.join();
}
