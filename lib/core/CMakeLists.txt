add_library(AirshipCore)
target_link_libraries(AirshipCore PUBLIC spdlog glfw AirshipRenderer AirshipCommonFlags)
enable_clang_tidy(AirshipCore)

option(AIRSHIP_INSTRUMENTATION "Enable instrumentation and trace logging" OFF)

if (AIRSHIP_INSTRUMENTATION)
    add_compile_definitions(AIRSHIP_INSTRUMENTATION)
endif()

set(AirshipCoreSources
    src/core/application.cpp
    src/core/event.cpp
    src/core/window.cpp
    src/core/instrumentation.cpp
)

set(AirshipCoreHeaders
    include/core/application.h
    include/core/convar.h
    include/core/event.h
    include/core/input.h
    include/core/instrumentation.h
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
target_sources(AirshipCore PUBLIC ${AirshipCoreHeaders})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    FILES
        ${AirshipCoreSources}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    FILES
        ${AirshipCoreHeaders}
)
