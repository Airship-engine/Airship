add_executable(FallingTrianglesExample)
set_target_properties(FallingTrianglesExample PROPERTIES FOLDER "Examples")
target_link_libraries(FallingTrianglesExample
    PUBLIC
        Airship
)

target_sources(FallingTrianglesExample PUBLIC main.cpp game.cpp)
target_sources(FallingTrianglesExample PUBLIC FILE_SET HEADERS
    FILES game.h
)
