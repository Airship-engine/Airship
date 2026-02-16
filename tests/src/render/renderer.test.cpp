// #include "core/application.h"
#include "render/opengl/renderer.h"

#include <string>
#include <vector>

#include "core/utils.hpp"
#include "core/window.h"
#include "gtest/gtest.h"
#include "test/common.h"

TEST(Renderer, Init) {
    // Use Application code to handle getting a window
    Airship::Test::GameClass app;
    app.Run();

    // We have a window, and it'll only be destroyed when app goes out of scope
    Airship::Window* window = app.GetWindow();
    EXPECT_TRUE(window != nullptr);
    EXPECT_TRUE(window->Get() != nullptr);

    // clang-format off
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    // clang-format on

    EXPECT_NO_THROW(Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource));
    Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource);

    // clang-format off
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";
    // clang-format on

    EXPECT_NO_THROW(Airship::Shader fragmentShader(Airship::ShaderType::Fragment, fragmentShaderSource));
    Airship::Shader fragmentShader(Airship::ShaderType::Fragment, fragmentShaderSource);

    EXPECT_NO_THROW(
        Airship::Pipeline pipeline(vertexShader, fragmentShader,
                                   {{.name = "Position", .location = 0, .format = Airship::ShaderDataType::Float3}}));
    Airship::Pipeline pipeline(vertexShader, fragmentShader,
                               {{.name = "Position", .location = 0, .format = Airship::ShaderDataType::Float3}});
    Airship::Material material(&pipeline);

    // Normalized device coordinates (NDC)
    // (-1,-1) lower-left corner, (1,1) upper-right
    using VertexType = Airship::Utils::Point<float, 3>;
    using VertexData = std::vector<VertexType>;
    const VertexData verticesA = {
        {-0.5f, -0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {0.0f, 0.5f, 0.0f},
    };
    Airship::Buffer verticesABuffer;
    verticesABuffer.update(verticesA.size() * sizeof(VertexType), verticesA.data());

    const VertexData verticesB = {
        {-0.5f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},
        {0.0f, -0.5f, 0.0f},
    };
    Airship::Buffer verticesBBuffer;
    verticesBBuffer.update(verticesB.size() * sizeof(VertexType), verticesB.data());

    std::vector<Airship::Mesh> meshes(2);
    meshes[0].setAttributeStream("Position", {.buffer = &verticesABuffer,
                                              .stride = sizeof(VertexType),
                                              .offset = 0,
                                              .format = Airship::ShaderDataType::Float3});
    meshes[0].setVertexCount(static_cast<int>(verticesA.size()));
    meshes[1].setAttributeStream("Position", {.buffer = &verticesBBuffer,
                                              .stride = sizeof(VertexType),
                                              .offset = 0,
                                              .format = Airship::ShaderDataType::Float3});
    meshes[1].setVertexCount(static_cast<int>(verticesB.size()));

    const auto& renderer = app.GetRenderer();

    // TODO: Pull into application?
    while (!window->shouldClose()) {
        window->pollEvents();

        // Draw code
        pipeline.bind();
        renderer.draw(meshes, material);

        // Show the rendered buffer
        window->swapBuffers();
    }

    // TODO: Test some API to verify results without
    // windows, for CI tests.
}
