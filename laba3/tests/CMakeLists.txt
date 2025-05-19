include(FetchContent)

# Загружаем GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# Убираем установки в систему
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(sandpile_tests tests.cpp)
target_link_libraries(sandpile_tests PRIVATE sandpile_lib gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(sandpile_tests)

