cmake_minimum_required(VERSION 3.17)
project(cpp_hw_1_string)
set(CMAKE_CXX_STANDARD 17)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(Test Source.hpp Tests.cpp)

target_link_libraries(Test  GTest::GTest GTest::Main)

add_test(
        NAME cpp_hw_1_string
        COMMAND cpp_hw_1_string
)