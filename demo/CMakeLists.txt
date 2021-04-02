add_executable(test0 test0.cpp)
target_link_libraries(test0 base reactor)

add_executable(test1 test1.cpp)
target_link_libraries(test1 base reactor)

add_executable(test2 test2.cpp)
target_link_libraries(test2 base reactor)

add_executable(test3 test3.cpp)
target_link_libraries(test3 base reactor)

add_executable(echo echo/echo.cpp echo/server.cpp)
target_link_libraries(echo base reactor)

add_executable(sudoku sudoku/server.cpp sudoku/sudoku.cpp)
target_link_libraries(sudoku base reactor)