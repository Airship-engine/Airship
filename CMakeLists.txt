# cmake guidelines inspired by https://cliutils.gitlab.io/modern-cmake
# minimum version 3.24 introduces a good package system, in case we need it. 
# our default compiler
cmake_minimum_required(VERSION 3.24)

project(
    Airship
    VERSION 0.0
    DESCRIPTION "Beginnings of a game engine"
)

# Check for minimum compiler versions
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC 13+ required for std::format
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13")
        message(FATAL_ERROR "Insufficient GCC version. 13 required")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang 17+ required for std::format
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17")
        message(FATAL_ERROR "Insufficient Clang version. 17 required")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # GCC 13+ required for std::format
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.29")
        message(FATAL_ERROR "Insufficient MSVC version. 19.29 required")
    endif()
else()
    message(FATAL_ERROR "Unsupported Compiler. Use GCC, Clang, or MSVC")
endif()


# Remove the unused ZERO_CHECK build target
set(CMAKE_SUPPRESS_REGENERATION true)

# We require c++20 across this whole project
set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED true)

option(BUILD_TESTS "Build and compile unit tests" OFF)
option(BUILD_FOR_CI "Build a CI version of the library" OFF)
option(OPENGL_DISABLED "Disallow the renderer to use the OpenGL API" FALSE)
option(BUILD_EXAMPLES "Build and compile examples" OFF)

if (BUILD_TESTS)
    enable_testing()
    include(CTest)
endif()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
