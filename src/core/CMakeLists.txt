
file(GLOB CORE_SOURCES
    *.cpp
)

add_library(core ${CORE_SOURCES})
target_compile_features(core PUBLIC cxx_std_20)
