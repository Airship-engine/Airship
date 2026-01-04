#include <optional>
#include <string>
#include <vector>

#include "core/application.h"
#include "core/window.h"
#include "render/opengl/renderer.h"

// clang-format off
const char* const vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* const fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";
// clang-format on

inline Airship::Pipeline createPipeline() {
    Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource);
    Airship::Shader fragmentShader(Airship::ShaderType::Fragment, fragmentShaderSource);
    return Airship::Pipeline(vertexShader, fragmentShader);
}
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

        Airship::Pipeline pipeline = createPipeline();

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

        std::vector<Airship::Mesh<Airship::VertexP>> meshes;
        meshes.emplace_back(verticesA);
        meshes.emplace_back(verticesB);

        // TODO: Pull into application?
        while (!m_MainWin.value()->shouldClose()) {
            m_MainWin.value()->pollEvents();

            // Draw code
            pipeline.bind();
            m_Renderer.draw(meshes);

            // Show the rendered buffer
            m_MainWin.value()->swapBuffers();
        }
    }

    int m_Height = 800, m_Width = 600;
    std::optional<Airship::Window*> m_MainWin;
    Airship::Renderer m_Renderer;
};
