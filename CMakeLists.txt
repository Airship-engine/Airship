# Arbitrary somewhat-recent cmake version
cmake_minimum_required(VERSION 3.20)

project(
    Airship
    VERSION 0.0
    DESCRIPTION "Beginnings of a game engine"
)

# Remove the unused ZERO_CHECK build target
set(CMAKE_SUPPRESS_REGENERATION true)

option(BUILD_TESTS "Build and compile unit tests" ON)

if (BUILD_TESTS)
    enable_testing()
    include(CTest)
endif()

add_subdirectory(External)
add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()
