project( nessndemu )
cmake_minimum_required( VERSION 3.14 )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BOOST_VERSION)

#add_definitions("-DHAVE_CONFIG_H")
include_directories(./)
add_library( ${PROJECT_NAME} STATIC apu_snapshot.cpp  Blip_Buffer.cpp  Multi_Buffer.cpp  Nes_Apu.cpp  Nes_Namco.cpp  Nes_Oscs.cpp  Nes_Vrc6.cpp  Nonlinear_Buffer.cpp )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)