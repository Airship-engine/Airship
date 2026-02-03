add_subdirectory(shapes)
add_subdirectory(falling_triangles)
add_subdirectory(snake)

add_custom_target(AirshipExamples)
set_target_properties(AirshipExamples PROPERTIES FOLDER "Examples")

add_dependencies(AirshipExamples ShapesExample)
add_dependencies(AirshipExamples FallingTrianglesExample)
add_dependencies(AirshipExamples SnakeExample)

