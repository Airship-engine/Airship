// #include "core/application.h"
#include "render/opengl/renderer.h"

#include <string>
#include <vector>

#include "core/window.h"
#include "gtest/gtest.h"
#include "render/color.h"
#include "test/common.h"

TEST(Renderer, Init) {
    // Use Application code to handle getting a window
    Airship::Test::GameClass app;
    app.Run();

    // We have a window, and it'll only be destroyed when app goes out of scope
    Airship::Window* window = nullptr;
    if (auto windowVar = app.GetWindow(); windowVar.has_value()) {
        window = windowVar.value();
    } else {
        FAIL() << "Failed to create window for rendering tests.";
    }
    EXPECT_TRUE(window != nullptr);
    EXPECT_TRUE(window->Get() != nullptr);

    // Should ultimately be handled inside of the application.
    Airship::Renderer r;
    r.init();
    r.setClearColor(Airship::Colors::CornflowerBlue);
    r.resize(window->GetSize().x(), window->GetSize().y());
    window->setWindowResizeCallback([&r](int width, int height) { r.resize(width, height); });

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

    Airship::Renderer::program_id pid;
    EXPECT_NO_THROW(pid = r.createPipeline(vertexShader, fragmentShader));

    // Normalized device coordinates (NDC)
    // (-1,-1) lower-left corner, (1,1) upper-right
    using VertexData = std::vector<Airship::VertexP>;
    const VertexData verticesA = {
        {{-0.5f, -0.5f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}},
        {{0.0f, 0.5f, 0.0f}},
    };

    const VertexData verticesB = {
        {{-0.5f, 0.5f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}},
        {{0.0f, -0.5f, 0.0f}},
    };

    std::vector<Airship::Mesh<Airship::VertexP>> meshes{verticesA, verticesB};

    // TODO: Pull into application?
    while (!window->shouldClose()) {
        window->pollEvents();

        // Draw code
        r.bindProgram(pid);
        r.draw(meshes);

        // Show the rendered buffer
        window->swapBuffers();
    }

    // TODO: Test some API to verify results without
    // windows, for CI tests.
}
