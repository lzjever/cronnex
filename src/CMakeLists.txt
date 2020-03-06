cmake_minimum_required(VERSION 3.0)

project(PCGT)

set(PACKAGE ${PROJECT_NAME})
set(PACKAGE_NAME ${PACKAGE})
set(PACKAGE_VERSION_MAJOR "0")
set(PACKAGE_VERSION_MINOR "1")
set(PACKAGE_VERSION_PATCH "0")
set(VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
set(PACKAGE_VERSION ${VERSION})
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB PCGT_SOURCES
    "*.cpp"
)

if (NOT TARGET raylib)
  find_package(raylib 2.0 REQUIRED)
endif()
add_executable(${PROJECT_NAME} ${PCGT_SOURCES} )
target_include_directories(${PROJECT_NAME} PUBLIC ${nes_snd_emu_includes})
target_link_libraries(${PROJECT_NAME} Nes_Snd_Emu raylib)