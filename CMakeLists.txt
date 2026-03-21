cmake_minimum_required(VERSION 3.16)
project(mini-redis CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Strict warnings for development.
add_compile_options(-Wall -Wextra -Wpedantic)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(mini-redis ${SOURCES})
target_link_libraries(mini-redis pthread)

enable_testing()
find_package(GTest QUIET)
if(GTest_FOUND)
  file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
  if(TEST_SOURCES)
    add_executable(run_tests ${TEST_SOURCES})
    target_link_libraries(run_tests GTest::gtest_main pthread)
    include(GoogleTest)
    gtest_discover_tests(run_tests)
  endif()
endif()
