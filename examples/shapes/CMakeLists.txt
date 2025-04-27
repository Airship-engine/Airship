add_executable(ShapesExample
    main.cpp
)
add_dependencies(AirshipExamples ShapesExample)

target_link_libraries(ShapesExample
    PUBLIC
        Airship
)

