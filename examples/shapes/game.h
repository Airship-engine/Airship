#include <optional>
#include <string>
#include <vector>

#include "core/application.h"
#include "core/window.h"
#include "render/opengl/renderer.h"

class Game : public Airship::Application {
public:
    Game() = default;
    Game(int height, int width) : m_Height(height), m_Width(width) {}

protected:
    void OnStart() override {
        m_MainWin = CreateWindow(m_Width, m_Height, "Shapes Example");

        m_Renderer.init();
        m_Renderer.resize(m_Width, m_Height);
        m_MainWin.value()->setWindowResizeCallback([this](int width, int height) {
            m_Renderer.resize(width, height);
            m_Height = height;
            m_Width = width;
        });

        // clang-format off
        const char* vertexShaderSource =
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";
        // clang-format on

        // clang-format off
        const char* fragmentShaderSource =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
            "}\0";
        // clang-format on

        Airship::Renderer::program_id pid;
        {
            // Shaders can be deleted after pipeline creation
            Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource);
            Airship::Shader fragmentShader(Airship::ShaderType::Fragment, fragmentShaderSource);
            pid = m_Renderer.createPipeline(vertexShader, fragmentShader);
        }

        // Normalized device coordinates (NDC)
        // (-1,-1) lower-left corner, (1,1) upper-right
        using VertexData = std::vector<Airship::VertexP>;
        VertexData verticesA = {
            {{-0.5f, -0.5f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}},
        };

        VertexData verticesB = {
            {{-0.5f, 0.5f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}},
            {{0.0f, -0.5f, 0.0f}},
        };

        std::vector<Airship::Mesh<Airship::VertexP>> meshes{verticesA, verticesB};

        // TODO: Pull into application?
        while (!m_MainWin.value()->shouldClose()) {
            m_MainWin.value()->pollEvents();

            // Draw code
            m_Renderer.bindProgram(pid);
            m_Renderer.draw(meshes);

            // Show the rendered buffer
            m_MainWin.value()->swapBuffers();
        }
    }

    int m_Height = 800, m_Width = 600;
    std::optional<Airship::Window*> m_MainWin;
    Airship::Renderer m_Renderer;
};
