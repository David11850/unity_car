cmake_minimum_required(VERSION 3.27.0)
project(UNITY)
include_directories(./include)

add_executable(epoll_sever_run src/run_epoll_sever.cpp src/manager.cpp src/sever.cpp)
add_executable(client_run src/run_client.cpp src/manager.cpp src/sever.cpp)