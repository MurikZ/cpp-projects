cmake_minimum_required(VERSION 3.16)
project(sandpile_app)

set(CMAKE_CXX_STANDARD 17)

# Исходники
add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(tests)
