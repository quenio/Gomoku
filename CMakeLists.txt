cmake_minimum_required(VERSION 3.3)
project(Gomoku)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++11 \
    -Werror \
    -Weverything \
    -Wno-c++98-compat -Wno-c++98-compat-pedantic \
    -Wno-padded \
    -Wno-weak-vtables \
    -Wno-header-hygiene \
    -Wno-global-constructors")

set(SOURCE_FILES main.cpp)
add_executable(Gomoku ${SOURCE_FILES})