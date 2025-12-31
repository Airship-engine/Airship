add_library(AirshipRenderer)
target_link_libraries(AirshipRenderer PUBLIC AirshipCore AirshipCommonFlags)
enable_clang_tidy(AirshipRenderer)

set(AirshipRendererSources
    src/render/color.cpp
)

set(AirshipRendererHeaders
    include/render/color.h
)

if (NOT OPENGL_DISABLED)
    find_package(OpenGL REQUIRED)

    list(APPEND AirshipRendererSources src/render/opengl/renderer.cpp)
    list(APPEND AirshipRendererHeaders include/render/opengl/renderer.h)
    target_link_libraries(AirshipRenderer PRIVATE gl3w OpenGL::GL)
else()
    message(FATAL_ERROR "Only the OpenGL renderer is supported.")
endif()

target_include_directories(AirshipRenderer
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(AirshipRenderer PRIVATE ${AirshipRendererSources})
target_sources(AirshipRenderer PUBLIC FILE_SET HEADERS
    BASE_DIRS include/render
    FILES ${AirshipRendererHeaders}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/render
    FILES
        ${AirshipRendererSources}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/render
    FILES
        ${AirshipRendererHeaders}
)
