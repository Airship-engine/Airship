#include "core/application.h"
#include "render/opengl/renderer.h"

class Game : public Airship::Application
{
public:
    Game() = default;
    Game(int height, int width) : m_Height(height), m_Width(width) {}

protected:
    void OnStart() override
    {
        m_MainWin = CreateWindow(m_Width, m_Height, "Shapes Example");

        m_Renderer.init();
        m_Renderer.resize(m_Width, m_Height);
        m_MainWin.value()->setWindowResizeCallback([this](int width, int height) {
            m_Renderer.resize(width, height);
            m_Height = height;
            m_Width = width;
        });

        const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

        Airship::Renderer::shader_id vs_id = m_Renderer.createShader(Airship::ShaderType::Vertex);
        bool ok = m_Renderer.compileShader(vs_id, vertexShaderSource);
        if (!ok) {
            std::string log = m_Renderer.getCompileLog(vs_id);
            SHIPLOG_ERROR(log);
        }

        const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

        Airship::Renderer::shader_id fs_id = m_Renderer.createShader(Airship::ShaderType::Fragment);
        ok = m_Renderer.compileShader(fs_id, fragmentShaderSource);
        if (!ok) {
            std::string log = m_Renderer.getCompileLog(fs_id);
            SHIPLOG_ERROR(log);
            return;
        }

        Airship::Renderer::program_id pid = m_Renderer.createProgram();
        m_Renderer.attachShader(pid, vs_id);
        m_Renderer.attachShader(pid, fs_id);
        ok = m_Renderer.linkProgram(pid);
        if (!ok) {
            std::string log = m_Renderer.getLinkLog(fs_id);
            SHIPLOG_ERROR(log);
        }
        m_Renderer.deleteShader(vs_id);
        m_Renderer.deleteShader(fs_id);

        // Normalized device coordinates (NDC)
        // (-1,-1) lower-left corner, (1,1) upper-right
        using VertexData = std::vector<Airship::Vertex>;
        VertexData verticesA = {
            {{-0.5f, -0.5f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}},
            {{ 0.0f,  0.5f, 0.0f}},
        };

        VertexData verticesB = {
            {{-0.5f,  0.5f, 0.0f}},
            {{ 0.5f,  0.5f, 0.0f}},
            {{ 0.0f, -0.5f, 0.0f}},
        };

        std::vector<Airship::Mesh> meshes {
            verticesA,
            verticesB
        };

        // TODO: Pull into application?
        while(!m_MainWin.value()->shouldClose())
        {
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
