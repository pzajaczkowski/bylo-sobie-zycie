cmake_minimum_required(VERSION 3.29)
project(bylo-sobie-zycie)

set(CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_subdirectory(src)
add_subdirectory(solutions/serial)
add_subdirectory(solutions/async)
add_subdirectory(solutions/async_block)
