
add_executable(ShapesExample)

target_link_libraries(ShapesExample
    PUBLIC
        Airship
)

target_sources(ShapesExample
    PRIVATE
        main.cpp
)

