# Собираем библиотеку из исходников
add_library(sandpile_lib
    Sandpile.cpp
    BmpWriter.cpp
)

target_include_directories(sandpile_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
