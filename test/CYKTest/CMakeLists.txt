cmake_minimum_required(VERSION 3.6)
project(CYKTest)
add_executable(CYKTest CYKTest.cpp)
target_link_libraries(CYKTest PUBLIC gtest pthread parser)