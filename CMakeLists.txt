cmake_minimum_required(VERSION 3.16)
project(testbncli C)

set(CMAKE_C_STANDARD 11)


include_directories(/usr/local/include)
link_directories(/usr/local/lib)
link_libraries(sodium toxcore)

add_executable(bn bn.c)
add_executable(cli cli.c)

