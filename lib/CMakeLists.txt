add_subdirectory(src/core)
add_subdirectory(src/render)

add_library(Airship)

target_link_libraries(Airship PUBLIC core AirshipRenderer)
