# Arbitrary somewhat-recent cmake version
cmake_minimum_required(VERSION 3.20)

project(
    Airship
    VERSION 0.0
    DESCRIPTION "Beginnings of a game engine"
)

# Remove the unused ZERO_CHECK build target
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(src)

option(BUILD_TESTS "Build and compile unit tests" ON)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
