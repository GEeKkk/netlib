#include "EventLoop.h"

int main(int argc, char const *argv[])
{
    // {
    //     EventLoop loop;
    //     loop.Loop();
    // }
    EventLoop loop;
    loop.Loop();
    EventLoop loop2;
    loop2.Loop();
    return 0;
}
