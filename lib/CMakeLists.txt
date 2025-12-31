add_subdirectory(core)
add_subdirectory(render)

add_library(Airship INTERFACE)

set(AirshipLibs 
    AirshipCore
    AirshipRenderer
)

# Target to link against for dependency tracking
target_link_libraries(Airship INTERFACE ${AirshipLibs})

# Preset utility target
add_custom_target(AllAirshipLibs)
add_dependencies(AllAirshipLibs ${AirshipLibs})

