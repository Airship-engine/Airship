
add_executable(ShapesExample)

target_link_libraries(ShapesExample
    PUBLIC
        core
        AirshipRenderer
)

target_sources(ShapesExample
    PRIVATE
        main.cpp
)