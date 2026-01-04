add_subdirectory(shapes)
add_subdirectory(falling_triangles)

add_custom_target(AirshipExamples)
set_target_properties(AirshipExamples PROPERTIES FOLDER "Examples")

add_dependencies(AirshipExamples ShapesExample)
add_dependencies(AirshipExamples FallingTrianglesExample)

