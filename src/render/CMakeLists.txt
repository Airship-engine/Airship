
add_library(AirshipRenderer)
target_include_directories(AirshipRenderer
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_sources(AirshipRenderer PUBLIC color.cpp)

if (NOT OPENGL_DISABLED)
    add_subdirectory(opengl)
else()
    message(FATAL_ERROR "Only the OpenGL renderer is supported.")
endif()

# Since renderer.test.cpp requires a window, skip for CI
if(NOT BUILD_FOR_CI)
    airship_test(renderer renderer.test.cpp)
endif()

add_airship_test("color.test.cpp")
