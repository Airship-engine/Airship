
find_package(OpenGL REQUIRED)

target_sources(AirshipRenderer PUBLIC renderer.cpp)
target_link_libraries(AirshipRenderer core gl3w OpenGL::GL)
