add_subdirectory(core)
add_subdirectory(render)

add_library(Airship INTERFACE)

target_link_libraries(Airship INTERFACE AirshipCore AirshipRenderer)
add_custom_target(AllAirshipLibs)
add_dependencies(AllAirshipLibs AirshipCore AirshipRenderer)
