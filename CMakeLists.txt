cmake_minimum_required(VERSION 3.29)
project(HNSW)

set(CMAKE_CXX_STANDARD 20)

add_executable(HNSW main.cpp
        Graph.cpp
        Graph.h
        Node.cpp
        Node.h)
