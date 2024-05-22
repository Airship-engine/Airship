if (BUILD_TESTS)
    set(INSTALL_GTEST OFF)
    add_subdirectory(googletest)
endif()

set(SPDLOG_INSTALL OFF)
set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(spdlog)
