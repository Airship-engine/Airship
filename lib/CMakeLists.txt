add_subdirectory(core)
add_subdirectory(render)

add_library(Airship)

target_link_libraries(Airship PUBLIC core AirshipRenderer)
