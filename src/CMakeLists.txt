cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT TARGET raylib)
  find_package(raylib 2.0 REQUIRED)
endif()

file(GLOB cronnex_SOURCES
    "*.cpp"
)

add_subdirectory(lib)
add_executable(${PROJECT_NAME}_gameplay ${cronnex_SOURCES} )
target_include_directories(${PROJECT_NAME}_gameplay PUBLIC ${nes_snd_emu_includes})
target_link_libraries(${PROJECT_NAME}_gameplay ${PROJECT_NAME}_core raylib)