#include <foxtail/reactor/EventLoop.h>
#include <thread>

using namespace std;

EventLoop *g_loop = nullptr;

// 在别的线程调用EventLoop
void Tester() {
    g_loop->Loop();
}

int main() {
    EventLoop loop;
    g_loop = &loop;
    thread th(Tester);
    th.join();
}
