if (BUILD_TESTS)
    set(INSTALL_GTEST OFF)
    add_subdirectory(googletest)
endif()

set(SPDLOG_INSTALL OFF)
set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(spdlog)

# XXX: See https://www.glfw.org/docs/latest/compile.html
# for additional dependencies for glfw on linux
if (UNIX)
    # Use wayland over X11, as its more modern
    set(GLFW_BUILD_X11 OFF)
    set(GLFW_BUILD_WAYLAND ON)
endif()
add_subdirectory(glfw)

if (NOT OPENGL_DISABLED)
    # Add gl3w - generated files added to the build/ directory
    set(OpenGL_GL_PREFERENCE LEGACY)
    add_subdirectory(gl3w)
endif()
