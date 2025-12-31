add_executable(ShapesExample)
set_target_properties(ShapesExample PROPERTIES FOLDER "Examples")
target_link_libraries(ShapesExample
    PUBLIC
        Airship
)
add_dependencies(AirshipExamples ShapesExample)

set(ShapesExampleSources
    main.cpp
)

set(ShapesExampleHeaders
    game.h
)

target_sources(ShapesExample PUBLIC ${ShapesExampleSources})
target_sources(ShapesExample PUBLIC FILE_SET HEADERS
    FILES ${ShapesExampleHeaders}
)

