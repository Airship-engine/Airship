include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)

if(BUILD_TESTS)
    add_custom_target(AirshipTests)
endif()

add_subdirectory(core)
add_subdirectory(render)

add_library(Airship)

target_link_libraries(Airship PUBLIC core AirshipRenderer)

