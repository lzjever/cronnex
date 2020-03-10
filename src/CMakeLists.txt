cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT TARGET raylib)
  find_package(raylib 2.0 REQUIRED)
endif()

file(GLOB game_play_source
    "*.cpp"
    "*.h"
)

add_subdirectory(nes)
add_executable(${PROJECT_NAME}_gameplay ${game_play_source} )
target_include_directories(${PROJECT_NAME}_gameplay PUBLIC ${nes_snd_emu_includes})
target_link_libraries(${PROJECT_NAME}_gameplay ${PROJECT_NAME}_nes raylib)

