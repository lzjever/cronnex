cmake_minimum_required(VERSION 3.0)
project(Nes_Snd_Emu VERSION 0.1.7)

file(GLOB NES_SND_EMU_SOURCES
    "nes_apu/*.cpp"
    #"*.cpp"
)

add_library(Nes_Snd_Emu STATIC ${NES_SND_EMU_SOURCES} )
set_target_properties(Nes_Snd_Emu PROPERTIES VERSION 0.1.7)
target_include_directories(Nes_Snd_Emu PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_definitions(Nes_Snd_Emu PUBLIC BLARGG_STATIC PRIVATE BLARGG_BUILD)