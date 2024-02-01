
file(GLOB CORE_SOURCES
    *.cpp
)

add_library(core ${CORE_SOURCES})
target_include_directories(core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/core/include
)
target_compile_features(core PUBLIC cxx_std_20)
