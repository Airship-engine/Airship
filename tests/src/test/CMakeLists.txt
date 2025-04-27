set(TEST_SOURCES
    common.cpp
)

add_library(AirshipTestUtils ${TEST_SOURCES})
target_include_directories(AirshipTestUtils
    PUBLIC
        ${PROJECT_SOURCE_DIR}/tests/include
)
target_link_libraries(AirshipTestUtils PUBLIC core
    core
    GTest::gtest_main
)
target_compile_features(AirshipTestUtils PUBLIC cxx_std_20)
