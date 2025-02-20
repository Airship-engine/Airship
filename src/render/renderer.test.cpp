// #include "core/application.h"
#include "gtest/gtest.h"
#include "core/logging.h"
#include "render/renderer.h"
#include "test/common.h"

TEST(Renderer, Init) {
    // Use Application code to handle getting a window
    GameClass app;
    app.Run();

    // We have a window, and it'll only be destroyed when app goes out of scope
    EXPECT_EQ(app.GetWindow().has_value(), true);
    Airship::Window *window = app.GetWindow().value();
    EXPECT_TRUE(window != nullptr);
    EXPECT_TRUE(window->Get() != nullptr);

    // Should ultimately be handled inside of the application.
    Airship::Renderer r;
    r.init();
    r.resize(window->GetSize().x(), window->GetSize().y());
    app.GetWindow().value()->setWindowResizeCallback([&r](int width, int height) {
        r.resize(width, height);
    });

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    Airship::shader_id vs_id = r.createShader(Airship::ShaderType::Vertex);
    bool ok = r.compileShader(vs_id, vertexShaderSource);
    EXPECT_TRUE(ok);
    if (!ok) {
        std::string log = r.getCompileLog(vs_id);
        SHIPLOG_ERROR(log);
    }

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    Airship::shader_id fs_id = r.createShader(Airship::ShaderType::Fragment);
    ok = r.compileShader(fs_id, fragmentShaderSource);
    EXPECT_TRUE(ok);
    if (!ok) {
        std::string log = r.getCompileLog(fs_id);
        SHIPLOG_ERROR(log);
    }

    Airship::program_id pid = r.createProgram();
    r.attachShader(pid, vs_id);
    r.attachShader(pid, fs_id);
    ok = r.linkProgram(pid);
    EXPECT_TRUE(ok);
    if (!ok) {
        std::string log = r.getLinkLog(fs_id);
        SHIPLOG_ERROR(log);
    }
    r.deleteShader(vs_id);
    r.deleteShader(fs_id);

    // Normalized device coordinates (NDC)
    // (-1,-1) lower-left corner, (1,1) upper-right
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    Airship::vao_id vao = r.createVertexArrayObject();
    r.bindVertexArrayObject(vao);

    Airship::buffer_id buf_id = r.createBuffer();
    r.bindBuffer(buf_id);
    r.copyBuffer(sizeof(vertices), &vertices);

    r.setVertexAttribDataFloat(0, 3);
    r.enableVertexAttribArray(0);

    // TODO: Pull into application?
    while(!window->shouldClose())
    {
        window->pollEvents();

        // Draw code
        r.bindProgram(pid);
        r.bindVertexArrayObject(vao);
        r.drawTriangles(0, 3);

        // Show the rendered buffer
        window->swapBuffers();
    }

    // TODO: Test some API to verify results without
    // windows, for CI tests.
}
