add_executable(main main.cpp)

target_link_libraries(main PRIVATE number)
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/lib)
