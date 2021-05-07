cmake_minimum_required(VERSION 3.17)
project(Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server main.cpp Administrator.cpp Administrator.h Controller.cpp Controller.h Buffer.cpp Buffer.h Cloud.cpp Cloud.h Cluster_Search.cpp Cluster_Search.h Field.cpp Field.h Point.cpp Point.h TreeNode.h Controller/buffer.cpp Controller/clustering.cpp Clustering/dbscan.cpp Clustering/dbscan.h Clustering/wave.cpp Clustering/wave.h Clustering/kmeans.cpp Clustering/kmeans.h Clustering/em.cpp Clustering/em.h Clustering/stree.cpp Clustering/stree.h Clustering/kmcores.cpp Clustering/kmcores.h Clustering/ha.cpp Clustering/ha.h Clustering/forel.cpp Clustering/forel.h Clustering/delaunay.cpp Clustering/delaunay.h Clustering/histogram.cpp Clustering/histogram.h)
find_package(Threads REQUIRED)
target_link_libraries(Server Threads::Threads)