cmake_minimum_required(VERSION 3.0)


add_executable(${PROJECT_NAME}_test TestMain.cpp)
target_link_libraries(${PROJECT_NAME}_test cronnex_core::testlib doctest)