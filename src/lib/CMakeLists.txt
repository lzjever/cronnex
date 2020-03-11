cmake_minimum_required(VERSION 3.0)


file(GLOB commonlib_source
    "common/*.cpp"
    "common/*.h"
)


file(GLOB neslib_source
    "nes/*.cpp"
    "nes/*.h"
)


add_library(${PROJECT_NAME}_lib ${neslib_source} ${commonlib_source})
target_compile_definitions(${PROJECT_NAME}_lib PRIVATE DOCTEST_CONFIG_DISABLE)
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}_lib Nes_Snd_Emu doctest)


# for doctest
add_library(${PROJECT_NAME}_testlib OBJECT ${neslib_source} ${commonlib_source})
target_include_directories(${PROJECT_NAME}_testlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}_testlib Nes_Snd_Emu doctest)


