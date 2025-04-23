cmake_minimum_required(VERSION 3.10)
project(WordCount)

set(CMAKE_CXX_STANDARD 17)

add_executable(WordCount main.cpp word.cpp)
