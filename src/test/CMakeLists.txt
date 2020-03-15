cmake_minimum_required(VERSION 3.0)


file(GLOB testcases_source
    "*.cpp"
)
add_executable(${PROJECT_NAME}_test ${testcases_source})
target_link_libraries(${PROJECT_NAME}_test cronnex_core::testlib doctest)