add_executable(async_solution src/main.cpp)
target_link_libraries(async_solution common)
target_link_libraries(async_solution ${MPI_CXX_LIBRARIES})