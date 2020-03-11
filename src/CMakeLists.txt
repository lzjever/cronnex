cmake_minimum_required(VERSION 3.0)


configure_file(cronnex_config.in.cmake config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(program)
add_subdirectory(test)