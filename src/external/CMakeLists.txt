cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

set(BUILD_EXAMPLES OFF CACHE BOOL "Build the examples")
set(BUILD_GAMES OFF CACHE BOOL "Build the example games.")
add_subdirectory(raylib)

#set(DOCTEST_WITH_TESTS OFF CACHE BOOL "Build tests/examples")
#set(DOCTEST_NO_INSTALL ON CACHE BOOL "Skip the installation process")
add_subdirectory(doctest)

add_subdirectory(Nes_Snd_Emu)