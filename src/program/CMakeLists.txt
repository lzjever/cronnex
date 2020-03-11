cmake_minimum_required(VERSION 3.0)


if (NOT TARGET raylib)
  find_package(raylib 2.0 REQUIRED)
endif()

file(GLOB game_play_source
    "*.cpp"
    "*.h"
)

add_definitions(-DDOCTEST_CONFIG_DISABLE)
add_executable(${PROJECT_NAME}_gameplay ${game_play_source} )
target_link_libraries(${PROJECT_NAME}_gameplay ${PROJECT_NAME}_lib raylib doctest)
