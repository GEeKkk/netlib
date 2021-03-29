#include "EventLoop.h"
#include "IOThread.h"
#include "netlib/base/CurrentThread.h"
#include <stdio.h>

void runInThread()
{
    printf("runInThread(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());
}

int main()
{
    printf("main(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());

    IOThread ioth;
    EventLoop *loop = ioth.StartLoop();
    // loop->runInLoop(runInThread);
    sleep(1);
    // loop->runAfter(2, runInThread);
    sleep(3);
    loop->Quit();

    printf("exit main().\n");
}