add_executable(
    unit_tests
    test.cpp
    )

target_link_libraries(
    unit_tests
    externalsort_lib
    gtest_main
    )

target_include_directories(unit_tests PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_test(
    NAME
        unit
    COMMAND
        ${CMAKE_BINARY_DIR}/test/unit_tests
    )

file(COPY ${PROJECT_SOURCE_DIR}/test/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

