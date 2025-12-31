add_subdirectory(shapes)

add_custom_target(AirshipExamples)
set_target_properties(AirshipExamples PROPERTIES FOLDER "Examples")

add_dependencies(AirshipExamples ShapesExample)

