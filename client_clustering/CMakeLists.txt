cmake_minimum_required(VERSION 3.17)
project(Client)

set(CMAKE_CXX_STANDARD 14)

add_executable(Client main.cpp Interface.cpp Interface.h Sender.cpp Sender.h)
find_package(Threads REQUIRED)
target_link_libraries(Client Threads::Threads)