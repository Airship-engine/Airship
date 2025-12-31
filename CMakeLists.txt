# cmake guidelines inspired by https://cliutils.gitlab.io/modern-cmake
# minimum version 3.24 introduces a good package system, in case we need it. 
# our default compiler
cmake_minimum_required(VERSION 3.24)

project(
    Airship
    VERSION 0.0
    DESCRIPTION "Beginnings of a game engine"
)

MESSAGE(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# We require c++20 across this whole project
add_library(AirshipCommonFlags INTERFACE)
target_compile_features(AirshipCommonFlags INTERFACE cxx_std_20)
target_compile_options(AirshipCommonFlags INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(CLANG_TIDY "Enable clang-tidy static analysis" OFF)
include(clang-tidy)

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

option(BUILD_FOR_CI "Build a CI version of the library" OFF)
option(OPENGL_DISABLED "Disallow the renderer to use the OpenGL API" FALSE)

option(BUILD_TESTS "Build and compile unit tests" ON)
if (BUILD_TESTS)
    enable_testing()
    include(CTest)
    add_subdirectory(tests)
endif()

add_subdirectory(external)
add_subdirectory(lib)

option(BUILD_EXAMPLES "Build and compile examples" ON)
if (BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()
