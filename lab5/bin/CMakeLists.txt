add_executable(lab5 main.cpp)

target_link_libraries(lab5 ITMLparse)
target_include_directories(lab5 PRIVATE ${PROJECT_SOURCE_DIR})
