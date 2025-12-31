add_library(AirshipCore)
target_link_libraries(AirshipCore PUBLIC spdlog glfw AirshipCommonFlags)
enable_clang_tidy(AirshipCore)

set(AirshipCoreSources
    src/core/application.cpp
    src/core/event.cpp
)

set(AirshipCoreHeaders
    include/core/application.h
    include/core/convar.h
    include/core/event.h
    include/core/logging.h
    include/core/utils.hpp
    include/core/window.h
)

target_include_directories(AirshipCore
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(AirshipCore PRIVATE ${AirshipCoreSources})
target_sources(AirshipCore PUBLIC FILE_SET HEADERS
    BASE_DIRS include/core
    FILES ${AirshipCoreHeaders}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    PREFIX ""
    FILES
        ${AirshipCoreSources}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    PREFIX ""
    FILES
        ${AirshipCoreHeaders}
)
