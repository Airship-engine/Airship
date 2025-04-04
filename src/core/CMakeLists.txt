
set(CORE_SOURCES
    application.cpp
    event.cpp
)

add_library(core ${CORE_SOURCES})
target_include_directories(core
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(core PUBLIC spdlog glfw)
target_compile_features(core PUBLIC cxx_std_20)
target_compile_options(core PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)


set(CORE_TEST_SOURCES
    convar.test.cpp
    event.test.cpp
)

if(NOT BUILD_FOR_CI)
    list(APPEND CORE_TEST_SOURCES application.test.cpp)
endif()

airship_test(core_test "${CORE_TEST_SOURCES}")
airship_test(logging logging.test.cpp DISABLE_ENVIRONMENT)
