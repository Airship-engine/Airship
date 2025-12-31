# Disable clang-tidy for external projects
set(CMAKE_CXX_CLANG_TIDY "")

function(get_all_targets DIR output_var)
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
    foreach(SUBDIR IN LISTS SUBDIRS)
        get_all_targets("${SUBDIR}" new_targets)
        LIST(APPEND TGTS ${new_targets})
    endforeach()
    set(${output_var} ${TGTS} PARENT_SCOPE)
endfunction()

get_all_targets(${CMAKE_CURRENT_SOURCE_DIR} _old_targets)

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
    set(OpenGL_GL_PREFERENCE LEGACY)

    include(FetchContent)

    FetchContent_Declare(
        gl3w
        GIT_REPOSITORY https://github.com/skaslev/gl3w
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(gl3w)

    target_include_directories(gl3w SYSTEM INTERFACE
        $<BUILD_INTERFACE:${gl3w_BINARY_DIR}/include>
    )
endif()

get_all_targets(${CMAKE_CURRENT_SOURCE_DIR} _new_targets)

set(EXTERNAL_TARGET_FOLDER "ThirdParty" CACHE STRING "Set default folder location for third party targets")
message("Target folder: ${EXTERNAL_TARGET_FOLDER}")

# Move all external targets to ThirdParty/
foreach (t IN LISTS _new_targets)
    if (NOT t IN_LIST _old_targets)
        get_target_property(f ${t} FOLDER)
        if (f)
            set_target_properties(${t} PROPERTIES FOLDER "${EXTERNAL_TARGET_FOLDER}/${f}")
        else()
            set_target_properties(${t} PROPERTIES FOLDER "${EXTERNAL_TARGET_FOLDER}")
        endif()
    endif()
endforeach()

