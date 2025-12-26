set(CORE_SOURCES
    src/core/application.cpp
    src/core/event.cpp
)

add_library(core ${CORE_SOURCES})
target_include_directories(core
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(core PUBLIC spdlog glfw AirshipCommonFlags)
target_compile_features(core PUBLIC cxx_std_20)
target_compile_options(core PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
