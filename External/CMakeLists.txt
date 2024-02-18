if (BUILD_TESTS)
    set(INSTALL_GTEST OFF)
    add_subdirectory(googletest)
endif()

set(FMT_GTEST_EXTERNAL ON)
add_subdirectory(fmt)

set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_INSTALL OFF)
add_subdirectory(spdlog)
