cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB cronnex_SOURCES
    "*.cpp"
)

add_library(${PROJECT_NAME}_nes ${cronnex_SOURCES} )
target_include_directories(${PROJECT_NAME}_nes PUBLIC ${nes_snd_emu_includes})
target_link_libraries(${PROJECT_NAME}_nes Nes_Snd_Emu)