cmake_minimum_required(VERSION 3.29)

add_library(common OBJECT board.cpp utils.cpp)
target_include_directories(common PUBLIC ${CMAKE_SOURCE_DIR}/include)