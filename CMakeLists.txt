cmake_minimum_required(VERSION 3.0)
project(cronnex VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(src/external)
add_subdirectory(src)
add_subdirectory(test)