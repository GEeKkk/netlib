set(reactor_src
  EventLoop.cpp
  Channel.cpp
  Poller.cpp
  Acceptor.cpp
  Buffer.cpp
  InetAddress.cc
  IOThread.cpp
  IOThreadPool.cpp
  Socket.cc
  SocketsOps.cc
  TcpConn.cpp
  TcpServer.cpp
  Timer.cpp
  TimerManager.cpp
  )

add_library(reactor ${reactor_src})
target_link_libraries(reactor base pthread)

install(TARGETS reactor DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/reactor)

add_subdirectory(tests)