#include <memory>
#include <string>
#include <vector>

#include "core/application.h"
#include "core/utils.hpp"
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
class Game : public Airship::Application {
public:
    Game() = default;

protected:
    void createPipeline() {
        Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource);
        Airship::Shader fragmentShader(Airship::ShaderType::Fragment, fragmentShaderSource);
        m_Pipeline = std::make_unique<Airship::Pipeline>(
            vertexShader, fragmentShader,
            std::vector<Airship::Pipeline::VertexAttributeDesc>{
                {.name = "Position", .location = 0, .format = Airship::ShaderDataType::Float3}});
        m_Material = std::make_unique<Airship::Material>(m_Pipeline.get());
    }
    void OnStart() override {

        createPipeline();

        // Normalized device coordinates (NDC)
        // (-1,-1) lower-left corner, (1,1) upper-right
        using VertexType = Airship::Utils::Point<float, 3>;
        using VertexData = std::vector<VertexType>;
        VertexData verticesA = {
            {-0.5f, -0.5f, 0.0f},
            {0.5f, -0.5f, 0.0f},
            {0.0f, 0.5f, 0.0f},
        };

        VertexData verticesB = {
            {-0.5f, 0.5f, 0.0f},
            {0.5f, 0.5f, 0.0f},
            {0.0f, -0.5f, 0.0f},
        };

        m_Buffers = std::vector<Airship::Buffer>(2);
        m_Buffers[0].update(verticesA.size() * sizeof(VertexType), verticesA.data());
        m_Buffers[1].update(verticesB.size() * sizeof(VertexType), verticesB.data());

        m_Meshes = std::vector<Airship::Mesh>(2);
        // NOLINTNEXTLINE(readability-container-data-pointer)
        m_Meshes[0].setAttributeStream("Position", {.buffer = &m_Buffers[0],
                                                    .stride = sizeof(VertexType),
                                                    .offset = 0,
                                                    .format = Airship::ShaderDataType::Float3});
        m_Meshes[0].setVertexCount(static_cast<int>(verticesA.size()));
        m_Meshes[1].setAttributeStream("Position", {.buffer = &m_Buffers[1],
                                                    .stride = sizeof(VertexType),
                                                    .offset = 0,
                                                    .format = Airship::ShaderDataType::Float3});
        m_Meshes[1].setVertexCount(static_cast<int>(verticesB.size()));
    }

    void OnGameLoop(float /*elapsed*/) override {
        // Draw code
        m_Pipeline->bind();
        m_Renderer.draw(m_Meshes, *m_Material);
    }

    std::vector<Airship::Buffer> m_Buffers;
    std::vector<Airship::Mesh> m_Meshes;
    std::unique_ptr<Airship::Pipeline> m_Pipeline;
    std::unique_ptr<Airship::Material> m_Material;
};
