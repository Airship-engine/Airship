
add_library(AirshipRenderer)
target_include_directories(AirshipRenderer
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_sources(AirshipRenderer PUBLIC src/render/color.cpp)
target_link_libraries(AirshipRenderer core)

if (NOT OPENGL_DISABLED)
    find_package(OpenGL REQUIRED)

    target_sources(AirshipRenderer PUBLIC src/render/opengl/renderer.cpp)
    target_link_libraries(AirshipRenderer gl3w OpenGL::GL)
else()
    message(FATAL_ERROR "Only the OpenGL renderer is supported.")
endif()
