#include "Channel.h"
#include "EventLoop.h"

#include "netlib/base/Timestamp.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop = nullptr;

void timeout(muduo::Timestamp receiveTime)
{
  printf("%s Timeout!\n", receiveTime.toFormattedString().c_str());
  g_loop->Quit();
}

int main()
{
  printf("%s started\n", muduo::Timestamp::now().toFormattedString().c_str());
  EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  Channel channel(&loop, timerfd);
  channel.SetReadCallback(timeout);
  channel.EnableRead();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.Loop();

  ::close(timerfd);
}
