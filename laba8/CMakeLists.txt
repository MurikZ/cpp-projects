cmake_minimum_required(VERSION 3.14)
project(laba8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем поддиректории
add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(tests)
