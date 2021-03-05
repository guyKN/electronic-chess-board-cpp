cmake_minimum_required(VERSION 3.13)
project(chessBoard)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-I/usr/local/include -L/usr/local/lib -lwiringPi")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# aaa
add_executable(chessBoard main.cpp Electronics.cpp LedThreadManager.cpp LedThreadManager.h)