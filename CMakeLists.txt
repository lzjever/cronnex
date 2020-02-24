project( pcgt )
cmake_minimum_required( VERSION 3.14 )



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BOOST_VERSION)
set(BOOST_COMPONENTS filesystem)

find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS ${BOOST_COMPONENTS})
find_package(X11 REQUIRED)

add_executable( ${PROJECT_NAME} Bus.cpp  Cartridge.cpp  CPU6502.cpp  main.cpp  Mapper_000.cpp  Mapper.cpp  olcPixelGameEngine.cpp  PPU2C02.cpp )

target_link_libraries(${PROJECT_NAME}
    ${X11_X11_LIB}
    GL
    pthread
    png
    Boost::boost
    Boost::filesystem
    )