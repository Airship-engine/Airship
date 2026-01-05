#include "game.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
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
const char* const vertexShaderSource =
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

const char* const bgFragmentShaderSource =
    "#version 330 core\n"
    "in vec4 vertexColor;\n"
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
    "   FragColor = rgbColor(vertexColor.r);\n"
    "}\0";
// clang-format on

namespace {
std::tuple<Airship::Pipeline, Airship::Pipeline> createPipelines() {
    Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource);
    Airship::Shader triangleFragmentShader(Airship::ShaderType::Fragment, triangleFragmentShaderSource);
    Airship::Shader bgFragmentShader(Airship::ShaderType::Fragment, bgFragmentShaderSource);
    return {Airship::Pipeline(vertexShader, triangleFragmentShader), Airship::Pipeline(vertexShader, bgFragmentShader)};
}

float randomRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    auto ret = dis(gen);
    return ret;
}

void initTriangle(Airship::VertexPC& v1, Airship::VertexPC& v2, Airship::VertexPC& v3, float bgHue) {
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

    v1.m_Position = {p1.x(), p1.y(), 0.0f};
    v2.m_Position = {p2.x(), p2.y(), 0.0f};
    v3.m_Position = {p3.x(), p3.y(), 0.0f};

    // Fixup: move off screen
    auto lowestY = std::min({v1.m_Position.y(), v2.m_Position.y(), v3.m_Position.y()});
    v1.m_Position.y() += 1 - lowestY;
    v2.m_Position.y() += 1 - lowestY;
    v3.m_Position.y() += 1 - lowestY;

    // Randomize X position
    auto minX = std::min({v1.m_Position.x(), v2.m_Position.x(), v3.m_Position.x()});
    auto maxX = std::max({v1.m_Position.x(), v2.m_Position.x(), v3.m_Position.x()});
    auto randX = randomRange(-1.0f - minX, 1.0f - maxX);
    v1.m_Position.x() += randX;
    v2.m_Position.x() += randX;
    v3.m_Position.x() += randX;

    auto hue = bgHue + randomRange(AVOIDANCE_DEGREES, 360.0f - AVOIDANCE_DEGREES);
    auto sat = randomRange(0.4f, 1.0f);
    auto val = randomRange(0.5f, 1.0f);
    auto color = Airship::HSVColor(hue, sat, val);
    v1.m_Color = color;
    v2.m_Color = color;
    v3.m_Color = color;
}
} // namespace

void Game::OnStart() {
    auto window = CreateWindow(m_Width, m_Height, "Falling triangles Example");
    assert(window.has_value());
    m_MainWin = window.value();

    m_Renderer.init();
    m_Renderer.resize(m_Width, m_Height);
    m_MainWin->setWindowResizeCallback([this](int width, int height) {
        m_Renderer.resize(width, height);
        m_Height = height;
        m_Width = width;
    });

    auto [triangles_pipeline, bg_pipeline] = createPipelines();

    auto bgMesh = Airship::Mesh<Airship::VertexPC>();
    auto triangleMesh = Airship::Mesh<Airship::VertexPC>();

    // Normalized device coordinates (NDC)
    // (-1,-1) lower-left corner, (1,1) upper-right
    auto bgHue = 0.0f; // At the top of the screen, where triangles are getting spawned
    // bg moves down at DOWN_VEL screen space/sec
    // This takes t = 2 / DOWN_VEL seconds
    // During this time, the hue rotates by HUE_ROTATION_SPEED * t degrees
    auto bottomBgHue = HUE_ROTATION_SPEED * 2 / DOWN_VEL;
    auto [v1, v2, v3] = bgMesh.addTriangle();
    v1.m_Position = {-1.0f, 1.0f, 0.0f};
    v2.m_Position = {1.0f, 1.0f, 0.0f};
    v3.m_Position = {-1.0f, -1.0f, 0.0f};
    v1.m_Color.r = bgHue;
    v2.m_Color.r = bgHue;
    v3.m_Color.r = bottomBgHue;
    auto [v4, v5, v6] = bgMesh.addTriangle();
    v4.m_Position = {1.0f, 1.0f, 0.0f};
    v5.m_Position = {1.0f, -1.0f, 0.0f};
    v6.m_Position = {-1.0f, -1.0f, 0.0f};
    v4.m_Color.r = bgHue;
    v5.m_Color.r = bottomBgHue;
    v6.m_Color.r = bottomBgHue;

    constexpr int maxTriangles = 20;
    int lowestTriangleIndex = 0;
    auto startTime = std::chrono::system_clock::now();
    float timeSinceSpawn = 0;
    while (!m_MainWin->shouldClose()) {
        auto frameTime = std::chrono::system_clock::now() - startTime;
        startTime = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration<float>(frameTime).count();
        elapsed = std::min(elapsed, 0.1f); // Clamp to avoid large jumps

        timeSinceSpawn += elapsed;
        if (timeSinceSpawn >= SPAWN_INTERVAL) {
            Airship::VertexPC *v1, *v2, *v3;
            bool skipSpawn = false;
            if (triangleMesh.getVertices().size() < maxTriangles * 3LU) {
                auto [newv1, newv2, newv3] = triangleMesh.addTriangle();
                v1 = &newv1;
                v2 = &newv2;
                v3 = &newv3;
            } else {
                v1 = &triangleMesh.getVertices()[(lowestTriangleIndex * 3) + 0];
                v2 = &triangleMesh.getVertices()[(lowestTriangleIndex * 3) + 1];
                v3 = &triangleMesh.getVertices()[(lowestTriangleIndex * 3) + 2];

                // Skip spawn if this triangle is still visible
                if (v1->m_Position.y() > -1.0f || v2->m_Position.y() > -1.0f || v3->m_Position.y() > -1.0f) {
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
        for (auto& vertex : triangleMesh.getVertices()) {
            vertex.m_Position.y() -= DOWN_VEL * elapsed;
            vertex.m_Color.a -= 0.1f * elapsed;
        }
        triangleMesh.invalidate();
        // Simulate moving down at DOWN_VEL speed

        bgHue += HUE_ROTATION_SPEED * elapsed;
        for (auto& vertex : bgMesh.getVertices()) {
            vertex.m_Color.r -= HUE_ROTATION_SPEED * elapsed;
        }
        bgMesh.invalidate();
        m_MainWin->pollEvents();

        // Draw code
        bg_pipeline.bind();
        m_Renderer.draw(bgMesh, bg_pipeline);

        triangles_pipeline.bind();
        m_Renderer.draw(triangleMesh, triangles_pipeline, false);

        // Show the rendered buffer
        m_MainWin->swapBuffers();
    }
}
