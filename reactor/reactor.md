# Reactor
- EventLoop
- Channel
- Poller

# EventLoop
一个事件循环，使用Poller的Poll等待事件发生，  
获取Poll监听的活跃的fd，并调用对应的事件处理函数进行处理。
```c++
while () {
    poller->Poll(timeout, activeChannels);
    for (acChan : activeChannels) {
        acChan->HandleEvent();
    }
}
```

# Channel
负责一个文件描述符fd的事件分发。  
一个Channel和一个fd为映射关系。  
将文件描述符和相应事件的处理函数注册到Channel中,
当事件发生时，由EventLoop调用
```c++
ReadCallback();
WriteCallback();
EnableRead();
EnableWrite();
```

# Poller
进行IO多路复用，由EventLoop持有并调用相关函数。
```c++
Poll();
GetActiveChannel();
UpdateChannel();
```

# 过程
当有新的描述符需要监听时，先将描述符和相应的处理函数封装到Channel中，  
再通过EventLoop::UpdateChannel->Poller::UpdateChannel加入到Poller的监听数组中。  
当Poller::Poll返回时，将活跃的描述符对应的Channel收集到一个数组中返回给EventLoop,  
由EventLoop调用Channel的处理函数对发生的事件进行处理。