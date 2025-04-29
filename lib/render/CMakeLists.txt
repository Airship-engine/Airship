
add_library(AirshipRenderer)
target_include_directories(AirshipRenderer
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_sources(AirshipRenderer PUBLIC src/render/color.cpp)

if (NOT OPENGL_DISABLED)
    add_subdirectory(src/opengl)
else()
    message(FATAL_ERROR "Only the OpenGL renderer is supported.")
endif()
