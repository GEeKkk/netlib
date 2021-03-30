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
  )

set(HEADERS
  Buffer.h
  Callbacks.h
  Channel.h
  EventLoop.h
  IOThread.h
  IOThreadPool.h
  InetAddress.h
  TcpConn.h
  TcpServer.h
  )


add_library(reactor ${reactor_src})
target_link_libraries(reactor base pthread)

install(TARGETS reactor DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/foxtail/reactor)