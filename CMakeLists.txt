cmake_minimum_required(VERSION 3.22)
project(bylo-sobie-zycie)

set(CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

option(USE_OPENMP "Enable OpenMP parallelization" OFF)
if(USE_OPENMP)
    find_package(OpenMP REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(solutions/serial)
add_subdirectory(solutions/async)
add_subdirectory(solutions/async_block)
