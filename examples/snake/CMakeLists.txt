add_executable(SnakeExample)
set_target_properties(SnakeExample PROPERTIES FOLDER "Examples")
target_link_libraries(SnakeExample
    PUBLIC
        Airship
)

target_sources(SnakeExample PUBLIC main.cpp game.cpp)
target_sources(SnakeExample PUBLIC FILE_SET HEADERS
    FILES game.h snake.h grid.h tallies.h
)

# Copy assets to the build tree
set(ASSET_SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(ASSET_DST_DIR $<TARGET_FILE_DIR:SnakeExample>/assets)

add_custom_target(SnakeExample_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSET_SRC_DIR}
            ${ASSET_DST_DIR}
    COMMENT "Copying assets to build directory"
)
add_dependencies(SnakeExample SnakeExample_assets)
