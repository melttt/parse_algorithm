cmake_minimum_required(VERSION 3.6)
project(chartTableTest)
add_executable(chartTableTest chartTableTest.cpp)
target_link_libraries(chartTableTest PUBLIC gtest pthread parser)