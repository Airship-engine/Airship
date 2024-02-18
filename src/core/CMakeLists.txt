
set(CORE_SOURCES
    ${PROJECT_SOURCE_DIR}/include/core/application.h
    ${PROJECT_SOURCE_DIR}/include/core/logging.h
    ${PROJECT_SOURCE_DIR}/include/core/window.h
    application.cpp
)

set(CORE_TEST_SOURCES
    application.test.cpp
    logging.test.cpp
)

add_library(core ${CORE_SOURCES})
target_include_directories(core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(core PUBLIC spdlog)
target_compile_features(core PUBLIC cxx_std_20)

foreach(TEST_SOURCE ${CORE_TEST_SOURCES})
    add_airship_test(${TEST_SOURCE})
endforeach()
