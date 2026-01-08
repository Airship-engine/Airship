#include "game.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "color.h"
#include "opengl/renderer.h"
#include "utils.hpp"

constexpr float DOWN_VEL = 0.2f; // Screen space / second
constexpr float HUE_ROTATION_SPEED = 8.0f; // Degrees / second
constexpr float SPAWN_INTERVAL = 0.5f; // Seconds between triangle spawns
constexpr float MIN_TRIANGLE_EXTENT = 0.1f; // Min distance from center to vertex
constexpr float MAX_TRIANGLE_EXTENT = 0.4f; // Max distance from center to vertex
constexpr float AVOIDANCE_DEGREES = 90.0f; // Hue degrees away from background for triangle spawn

// clang-format off
const char* const triangleVertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "out vec4 vertexColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   vertexColor = aColor;\n"
    "}\0";

const char* const triangleFragmentShaderSource =
    "#version 330 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n"
    "}\0";

const char* const bgVertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in float aHue;\n"
    "out float vertexHue;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   vertexHue = aHue;\n"
    "}\0";

const char* const bgFragmentShaderSource =
    "#version 330 core\n"
    "in float vertexHue;\n"
    "out vec4 FragColor;\n"
    "\n"
    "vec4 rgbColor(float hue) {\n"
    "    float sat = 0.6f;\n"
    "    float val = 0.6f;\n"
    "    hue = mod(hue, 360.0f);\n"
    "    float chroma = sat * val;\n"
    "    float X = chroma * (1 - abs(mod(hue / 60, 2) - 1));\n"
    "    float m = val - chroma;\n"
    "    float r, g, b;\n"
    "    if (hue < 60)\n"
    "        r = chroma, g = X, b = 0;\n"
    "    else if (hue < 120)\n"
    "        r = X, g = chroma, b = 0;\n"
    "    else if (hue < 180)\n"
    "        r = 0, g = chroma, b = X;\n"
    "    else if (hue < 240)\n"
    "        r = 0, g = X, b = chroma;\n"
    "    else if (hue < 300)\n"
    "        r = X, g = 0, b = chroma;\n"
    "    else\n"
    "        r = chroma, g = 0, b = X;\n"
    "\n"
    "    r += m;\n"
    "    g += m;\n"
    "    b += m;\n"
    "    return vec4(r, g, b, 1.0f);\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = rgbColor(vertexHue);\n"
    "}\0";
// clang-format on

namespace {
std::tuple<Airship::Pipeline, Airship::Pipeline> createPipelines() {
    Airship::Shader triangleVertexShader(Airship::ShaderType::Vertex, triangleVertexShaderSource);
    Airship::Shader triangleFragmentShader(Airship::ShaderType::Fragment, triangleFragmentShaderSource);
    Airship::Shader bgVertexShader(Airship::ShaderType::Vertex, bgVertexShaderSource);
    Airship::Shader bgFragmentShader(Airship::ShaderType::Fragment, bgFragmentShaderSource);
    return {Airship::Pipeline(triangleVertexShader, triangleFragmentShader,
                              {
                                  {"Position", 0, Airship::VertexFormat::Float3},
                                  {"Color", 1, Airship::VertexFormat::Float4},
                              }),
            Airship::Pipeline(bgVertexShader, bgFragmentShader,
                              {
                                  {"Position", 0, Airship::VertexFormat::Float3},
                                  {"Hue", 1, Airship::VertexFormat::Float},
                              })};
}

float randomRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    auto ret = dis(gen);
    return ret;
}

using vec3 = Airship::Utils::Point<float, 3>;
struct TriangleVertexData {
    vec3 position;
    Airship::Color color;
};

void initTriangle(TriangleVertexData& v1, TriangleVertexData& v2, TriangleVertexData& v3, float bgHue) {
    constexpr float pi = std::numbers::pi_v<float>;
    // On average, each point rotates by 2pi/3
    constexpr float minAngle = pi / 3; // valid range: [0, 2pi/3]
    auto ang1 = randomRange(0.0f, 2.0f * pi);
    auto ang2 = ang1 + randomRange(minAngle, pi - (minAngle / 2));
    auto ang3 = ang2 + randomRange(minAngle, pi - (minAngle / 2));

    auto size = randomRange(MIN_TRIANGLE_EXTENT, MAX_TRIANGLE_EXTENT);
    auto p1 = Airship::Utils::Point<float, 2>{std::cos(ang1) * size, std::sin(ang1) * size};
    auto p2 = Airship::Utils::Point<float, 2>{std::cos(ang2) * size, std::sin(ang2) * size};
    auto p3 = Airship::Utils::Point<float, 2>{std::cos(ang3) * size, std::sin(ang3) * size};

    v1.position = {p1.x(), p1.y(), 0.0f};
    v2.position = {p2.x(), p2.y(), 0.0f};
    v3.position = {p3.x(), p3.y(), 0.0f};

    // Fixup: move off screen
    auto lowestY = std::min({v1.position.y(), v2.position.y(), v3.position.y()});
    v1.position.y() += 1 - lowestY;
    v2.position.y() += 1 - lowestY;
    v3.position.y() += 1 - lowestY;

    // Randomize X position
    auto minX = std::min({v1.position.x(), v2.position.x(), v3.position.x()});
    auto maxX = std::max({v1.position.x(), v2.position.x(), v3.position.x()});
    auto randX = randomRange(-1.0f - minX, 1.0f - maxX);
    v1.position.x() += randX;
    v2.position.x() += randX;
    v3.position.x() += randX;

    auto hue = bgHue + randomRange(AVOIDANCE_DEGREES, 360.0f - AVOIDANCE_DEGREES);
    auto sat = randomRange(0.4f, 1.0f);
    auto val = randomRange(0.5f, 1.0f);
    auto color = Airship::HSVColor(hue, sat, val);
    v1.color = color;
    v2.color = color;
    v3.color = color;
}
} // namespace

void Game::OnStart() {
    m_Renderer.init();
    m_Renderer.resize(m_Width, m_Height);
    m_MainWindow->setWindowResizeCallback([this](int width, int height) {
        m_Renderer.resize(width, height);
        m_Height = height;
        m_Width = width;
    });

    auto [triangles_pipeline, bg_pipeline] = createPipelines();

    // Normalized device coordinates (NDC)
    // (-1,-1) lower-left corner, (1,1) upper-right
    auto bgHue = 0.0f; // At the top of the screen, where triangles are getting spawned
    // bg moves down at DOWN_VEL screen space/sec
    // This takes t = 2 / DOWN_VEL seconds
    // During this time, the hue rotates by HUE_ROTATION_SPEED * t degrees
    auto bottomBgHue = HUE_ROTATION_SPEED * 2 / DOWN_VEL;
    std::vector<vec3> bgPositions = {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},  {-1.0f, -1.0f, 0.0f},
                                     {1.0f, 1.0f, 0.0f},  {1.0f, -1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f}};
    std::vector<float> bgHues = {bgHue, bgHue, bottomBgHue, bgHue, bottomBgHue, bottomBgHue};
    // Will be updated at draw time
    std::vector<TriangleVertexData> triangleVertices{};

    Airship::Buffer triangleBuffer, bgPositionsBuffer, bgHuesBuffer;
    bgPositionsBuffer.update(bgPositions.size() * sizeof(vec3), bgPositions.data());
    bgHuesBuffer.update(bgHues.size() * sizeof(float), bgHues.data());

    Airship::Mesh bgMesh, triangleMesh;
    bgMesh.setAttributeStream(
        "Position",
        {.buffer = &bgPositionsBuffer, .stride = sizeof(vec3), .offset = 0, .format = Airship::VertexFormat::Float3});
    bgMesh.setAttributeStream(
        "Hue", {.buffer = &bgHuesBuffer, .stride = sizeof(float), .offset = 0, .format = Airship::VertexFormat::Float});
    bgMesh.setVertexCount(6);
    triangleMesh.setAttributeStream("Position", {.buffer = &triangleBuffer,
                                                 .stride = sizeof(TriangleVertexData),
                                                 .offset = offsetof(TriangleVertexData, position),
                                                 .format = Airship::VertexFormat::Float3});
    triangleMesh.setAttributeStream("Color", {.buffer = &triangleBuffer,
                                              .stride = sizeof(TriangleVertexData),
                                              .offset = offsetof(TriangleVertexData, color),
                                              .format = Airship::VertexFormat::Float4});

    constexpr int maxTriangles = 20;
    int lowestTriangleIndex = 0;
    auto startTime = std::chrono::system_clock::now();
    float timeSinceSpawn = 0;
    while (!m_MainWindow->shouldClose()) {
        auto frameTime = std::chrono::system_clock::now() - startTime;
        startTime = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration<float>(frameTime).count();
        elapsed = std::min(elapsed, 0.1f); // Clamp to avoid large jumps

        timeSinceSpawn += elapsed;
        if (timeSinceSpawn >= SPAWN_INTERVAL) {
            TriangleVertexData *v1, *v2, *v3;
            bool skipSpawn = false;
            if (triangleMesh.vertexCount() < maxTriangles * 3) {
                triangleVertices.emplace_back();
                triangleVertices.emplace_back();
                triangleVertices.emplace_back();
                triangleMesh.setVertexCount(static_cast<int>(triangleVertices.size()));
                v1 = &triangleVertices[triangleVertices.size() - 3];
                v2 = &triangleVertices[triangleVertices.size() - 2];
                v3 = &triangleVertices[triangleVertices.size() - 1];
            } else {
                v1 = &triangleVertices[(lowestTriangleIndex * 3) + 0];
                v2 = &triangleVertices[(lowestTriangleIndex * 3) + 1];
                v3 = &triangleVertices[(lowestTriangleIndex * 3) + 2];

                // Skip spawn if this triangle is still visible
                if (v1->position.y() > -1.0f || v2->position.y() > -1.0f || v3->position.y() > -1.0f) {
                    skipSpawn = true;
                } else {
                    lowestTriangleIndex = (lowestTriangleIndex + 1) % maxTriangles;
                }
            }
            if (!skipSpawn) {
                initTriangle(*v1, *v2, *v3, bgHue);
                timeSinceSpawn = 0;
            }
        }
        for (auto& vertex : triangleVertices) {
            vertex.position.y() -= DOWN_VEL * elapsed;
            vertex.color.a -= 0.1f * elapsed;
        }
        triangleBuffer.update(triangleVertices.size() * sizeof(TriangleVertexData), triangleVertices.data());
        // Simulate moving down at DOWN_VEL speed

        bgHue -= HUE_ROTATION_SPEED * elapsed;
        for (auto& hue : bgHues) {
            hue -= HUE_ROTATION_SPEED * elapsed;
        }
        bgHuesBuffer.update(bgHues.size() * sizeof(float), bgHues.data());
        m_MainWindow->pollEvents();

        // Draw code
        bg_pipeline.bind();
        m_Renderer.draw(bgMesh, bg_pipeline);

        triangles_pipeline.bind();
        m_Renderer.draw(triangleMesh, triangles_pipeline, false);

        // Show the rendered buffer
        m_MainWindow->swapBuffers();
    }
}
