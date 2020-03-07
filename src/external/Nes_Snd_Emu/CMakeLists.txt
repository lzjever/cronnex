cmake_minimum_required(VERSION 3.0)

project(Nes_Snd_Emu)

set(PACKAGE "Nes_Snd_Emu")
set(PACKAGE_NAME ${PACKAGE})
set(PACKAGE_VERSION_MAJOR "0")
set(PACKAGE_VERSION_MINOR "1")
set(PACKAGE_VERSION_PATCH "7")
set(VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
set(PACKAGE_VERSION ${VERSION})
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB NES_SND_EMU_SOURCES
    "nes_apu/*.cpp"
    #"*.cpp"
)

set(nes_snd_emu_includes ${PROJECT_SOURCE_DIR})
add_library(Nes_Snd_Emu STATIC ${NES_SND_EMU_SOURCES} )
set_target_properties(Nes_Snd_Emu PROPERTIES VERSION ${VERSION})
target_include_directories(Nes_Snd_Emu PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_definitions(Nes_Snd_Emu PUBLIC BLARGG_STATIC PRIVATE BLARGG_BUILD)