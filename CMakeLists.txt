cmake_minimum_required(VERSION 3.27.0)
project(UNITY)
include_directories(./include)

add_executable(epoll run_epoll_sever.cpp src/manager.cpp src/sever.cpp)
add_executable(client run_client.cpp src/manager.cpp src/sever.cpp)
