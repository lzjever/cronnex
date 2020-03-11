cmake_minimum_required(VERSION 3.0)
project(cronnex_core VERSION 0.0.1)

file(GLOB commonlib_source
    "common/*.cpp"
    "common/*.h"
)


file(GLOB neslib_source
    "nes/*.cpp"
    "nes/*.h"
)


add_library(cronnex_core_lib ${neslib_source} ${commonlib_source})
add_library( ${PROJECT_NAME}::lib ALIAS cronnex_core_lib )
target_compile_definitions(cronnex_core_lib PRIVATE DOCTEST_CONFIG_DISABLE)
target_include_directories(cronnex_core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cronnex_core_lib Nes_Snd_Emu doctest )


# for doctest
add_library(cronnex_core_testlib OBJECT ${neslib_source} ${commonlib_source})
add_library( ${PROJECT_NAME}::testlib ALIAS cronnex_core_testlib )
target_include_directories(cronnex_core_testlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cronnex_core_testlib Nes_Snd_Emu doctest )

