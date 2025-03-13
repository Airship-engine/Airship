
set(CORE_SOURCES
    application.cpp
    event.cpp
)

set(CORE_TEST_SOURCES
    convar.test.cpp
    event.test.cpp
)

if(NOT BUILD_FOR_CI)
    list(APPEND CORE_TEST_SOURCES application.test.cpp)
endif()

add_library(core ${CORE_SOURCES})
target_include_directories(core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(core PUBLIC spdlog glfw)
target_compile_features(core PUBLIC cxx_std_20)

airship_test(core_test "${CORE_TEST_SOURCES}")
airship_Test(logging logging.test.cpp DISABLE_ENVIRONMENT)
