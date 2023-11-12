cmake_minimum_required(VERSION 3.6)
project(UngerTest)
add_executable(UngerTest UngerTest.cpp)
target_link_libraries(UngerTest PUBLIC gtest pthread parser)