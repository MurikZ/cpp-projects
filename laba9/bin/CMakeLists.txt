set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab8 main.cpp)

target_link_libraries(lab8 LazyAdapters)
target_include_directories(lab8 PRIVATE ${PROJECT_SOURCE_DIR})
