cmake_minimum_required(VERSION 3.17)
project(Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server main.cpp Administrator.cpp Administrator.h Controller.cpp Controller.h Buffer.cpp Buffer.h Cloud.cpp Cloud.h Cluster_Search.cpp Cluster_Search.h Field.cpp Field.h Point.cpp Point.h TreeNode.h)
find_package(Threads REQUIRED)
target_link_libraries(Server Threads::Threads)