set(base_src
  CurrentThread.cc
  Logging.cc
  LogStream.cc
  Timestamp.cc
  )

# 将指定的源文件生成链接文件
add_library(base ${base_src})
# 将base文件和pthread库链接
target_link_libraries(base pthread)
# 将目标文件加入到目的目录中
install(TARGETS base DESTINATION lib)

file(GLOB HEADERS "*.h")
# 安装头文件
install(FILES ${HEADERS} DESTINATION include/base)