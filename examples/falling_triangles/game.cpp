#include "game.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <numbers>
#include <random>
#include <string>
#include <vector>

#include "color.h"
#include "input.h"
#include "opengl/renderer.h"
#include "utils.hpp"
#include "window.h"

constexpr float DOWN_VEL = 0.2f; // Screen space / second
constexpr float HUE_ROTATION_SPEED = 8.0f; // Degrees / second
constexpr float SPAWN_INTERVAL = 0.5f; // Seconds between triangle spawns
constexpr float MIN_TRIANGLE_EXTENT = 0.1f; // Min distance from center to vertex
constexpr float MAX_TRIANGLE_EXTENT = 0.4f; // Max distance from center to vertex
constexpr float AVOIDANCE_DEGREES = 90.0f; // Hue degrees away from background for triangle spawn
constexpr float FAST_MODE_MULT = 3.0f; // Multiplier for hue rotation and falling speed in fast mode

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

void Game::OnKeyPress(const Airship::Window& /*window*/, Airship::Input::Key key, int /*scancode*/,
                      Airship::Input::KeyAction action, Airship::Input::KeyMods /*mods*/) {
    if (key == Airship::Input::Key::Space) {
        if (action == Airship::Input::KeyAction::Press)
            m_MoveFast = true;
        else if (action == Airship::Input::KeyAction::Release)
            m_MoveFast = false;
    }
};

void Game::CreatePipelines() {
    Airship::Shader triangleVertexShader(Airship::ShaderType::Vertex, triangleVertexShaderSource);
    Airship::Shader triangleFragmentShader(Airship::ShaderType::Fragment, triangleFragmentShaderSource);
    Airship::Shader bgVertexShader(Airship::ShaderType::Vertex, bgVertexShaderSource);
    Airship::Shader bgFragmentShader(Airship::ShaderType::Fragment, bgFragmentShaderSource);
    m_TriPipeline = std::make_unique<Airship::Pipeline>(triangleVertexShader, triangleFragmentShader,
                                                        std::vector<Airship::Pipeline::VertexAttributeDesc>{
                                                            {"Position", 0, Airship::VertexFormat::Float3},
                                                            {"Color", 1, Airship::VertexFormat::Float4},
                                                        });
    m_BGPipeline = std::make_unique<Airship::Pipeline>(bgVertexShader, bgFragmentShader,
                                                       std::vector<Airship::Pipeline::VertexAttributeDesc>{
                                                           {"Position", 0, Airship::VertexFormat::Float3},
                                                           {"Hue", 1, Airship::VertexFormat::Float},
                                                       });
}

namespace {
float randomRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    auto ret = dis(gen);
    return ret;
}

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
    CreatePipelines();
    m_TriBuffer = std::make_unique<Airship::Buffer>();
    m_BGHuesBuffer = std::make_unique<Airship::Buffer>();
    m_BGBuffer = std::make_unique<Airship::Buffer>();

    // bg moves down at DOWN_VEL screen space/sec
    // This takes t = 2 / DOWN_VEL seconds
    // During this time, the hue rotates by HUE_ROTATION_SPEED * t degrees
    auto bottomBgHue = HUE_ROTATION_SPEED * 2 / DOWN_VEL;
    m_BGPositions = {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},  {-1.0f, -1.0f, 0.0f},
                     {1.0f, 1.0f, 0.0f},  {1.0f, -1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f}};
    m_BGHues = {m_BGHue, m_BGHue, bottomBgHue, m_BGHue, bottomBgHue, bottomBgHue};

    m_BGBuffer->update(m_BGPositions.size() * sizeof(vec3), m_BGPositions.data());
    m_BGHuesBuffer->update(m_BGHues.size() * sizeof(float), m_BGHues.data());

    m_BGMesh.setAttributeStream(
        "Position",
        {.buffer = m_BGBuffer.get(), .stride = sizeof(vec3), .offset = 0, .format = Airship::VertexFormat::Float3});
    m_BGMesh.setAttributeStream(
        "Hue",
        {.buffer = m_BGHuesBuffer.get(), .stride = sizeof(float), .offset = 0, .format = Airship::VertexFormat::Float});
    m_BGMesh.setVertexCount(6);
    m_TriMesh.setAttributeStream("Position", {.buffer = m_TriBuffer.get(),
                                              .stride = sizeof(TriangleVertexData),
                                              .offset = offsetof(TriangleVertexData, position),
                                              .format = Airship::VertexFormat::Float3});
    m_TriMesh.setAttributeStream("Color", {.buffer = m_TriBuffer.get(),
                                           .stride = sizeof(TriangleVertexData),
                                           .offset = offsetof(TriangleVertexData, color),
                                           .format = Airship::VertexFormat::Float4});
}

void Game::OnGameLoop(float elapsed) {
    constexpr int maxTriangles = 20;
    elapsed = std::min(elapsed, 0.1f); // Clamp to avoid large jumps
    if (m_MoveFast) elapsed *= FAST_MODE_MULT;

    m_TimeSinceSpawn += elapsed;
    if (m_TimeSinceSpawn >= SPAWN_INTERVAL) {
        TriangleVertexData *v1, *v2, *v3;
        bool skipSpawn = false;
        if (m_TriMesh.vertexCount() < maxTriangles * 3) {
            m_TriVerts.emplace_back();
            m_TriVerts.emplace_back();
            m_TriVerts.emplace_back();
            m_TriMesh.setVertexCount(static_cast<int>(m_TriVerts.size()));
            v1 = &m_TriVerts[m_TriVerts.size() - 3];
            v2 = &m_TriVerts[m_TriVerts.size() - 2];
            v3 = &m_TriVerts[m_TriVerts.size() - 1];
        } else {
            v1 = &m_TriVerts[(m_LowestTriangleIndex * 3) + 0];
            v2 = &m_TriVerts[(m_LowestTriangleIndex * 3) + 1];
            v3 = &m_TriVerts[(m_LowestTriangleIndex * 3) + 2];

            // Skip spawn if this triangle is still visible
            if (v1->position.y() > -1.0f || v2->position.y() > -1.0f || v3->position.y() > -1.0f) {
                skipSpawn = true;
            } else {
                m_LowestTriangleIndex = (m_LowestTriangleIndex + 1) % maxTriangles;
            }
        }
        if (!skipSpawn) {
            initTriangle(*v1, *v2, *v3, m_BGHue);
            m_TimeSinceSpawn = 0;
        }
    }
    for (auto& vertex : m_TriVerts) {
        vertex.position.y() -= DOWN_VEL * elapsed;
        vertex.color.a -= 0.1f * elapsed;
    }
    m_TriBuffer->update(m_TriVerts.size() * sizeof(TriangleVertexData), m_TriVerts.data());
    // Simulate moving down at DOWN_VEL speed

    m_BGHue -= HUE_ROTATION_SPEED * elapsed;
    for (auto& hue : m_BGHues) {
        hue -= HUE_ROTATION_SPEED * elapsed;
    }
    m_BGHuesBuffer->update(m_BGHues.size() * sizeof(float), m_BGHues.data());

    // Draw code
    m_BGPipeline->bind();
    m_Renderer->draw(m_BGMesh, *m_BGPipeline);

    m_TriPipeline->bind();
    m_Renderer->draw(m_TriMesh, *m_TriPipeline, false);
}
