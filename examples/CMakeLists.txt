add_subdirectory(shapes)

add_custom_target(AirshipExamples)

add_dependencies(AirshipExamples ShapesExample)

