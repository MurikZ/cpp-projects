include(FetchContent)

# Подтягиваем GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

# Не устанавливаем в систему
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(bst_test test_bst_containers.cpp)
target_include_directories(bst_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(bst_test gtest_main)

include(GoogleTest)
gtest_discover_tests(bst_test)
