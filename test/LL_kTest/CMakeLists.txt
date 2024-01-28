cmake_minimum_required(VERSION 3.6)
project(LL_kTest)
add_executable(LL_kTest LL_kTest.cpp)
target_link_libraries(LL_kTest PUBLIC gtest pthread parser)
