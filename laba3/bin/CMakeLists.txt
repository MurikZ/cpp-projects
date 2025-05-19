# Основной исполняемый файл
add_executable(sandpile_app main.cpp)

# Линкуем библиотеку
target_link_libraries(sandpile_app PRIVATE sandpile_lib)
