cmake_minimum_required(VERSION 3.27)
project(lb1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE -D_UNICODE")

add_executable(lb1 main.cpp file-reverse.cpp)
