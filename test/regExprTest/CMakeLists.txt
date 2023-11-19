cmake_minimum_required(VERSION 3.6)
project(RegExprTest)
add_executable(RegExprTest RegExprTest.cpp)
target_link_libraries(RegExprTest PUBLIC gtest pthread parser)