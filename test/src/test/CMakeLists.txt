set(TEST_SOURCES
	common.cpp
)

add_library(AirshipTest ${TEST_SOURCES})
target_include_directories(AirshipTest
    PUBLIC
        ${PROJECT_SOURCE_DIR}/test/include
)
target_link_libraries(AirshipTest PUBLIC core
	core
	gtest_main
)
target_compile_features(AirshipTest PUBLIC cxx_std_20)
