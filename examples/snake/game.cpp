#include "game.h"

#include <cassert>
#include <chrono>
#include <memory>
#include <vector>

#include "addons.h"
#include "core/input.h"
#include "core/logging.h"
#include "core/window.h"
#include "grid.h"
#include "render/color.h"
#include "render/opengl/renderer.h"
#include "snake.h"

#define UNUSED(x) ((void) x)

constexpr float FRAME_TIME = 0.1f;
constexpr float LINE_WIDTH = 0.005f;

// clang-format off
const char* const vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "out vec4 vertexColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "   vertexColor = aColor;\n"
    "}\0";

const char* const fragmentShaderSource =
    "#version 330 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n"
    "}\0";
// clang-format on

void Game::CreatePipelines() {
    // Vertex-colored pipeline
    Airship::Shader vertexShader(Airship::ShaderType::Vertex, vertexShaderSource);
    Airship::Shader fragmentShader(Airship::ShaderType::Fragment, fragmentShaderSource);
    m_Pipeline = std::make_unique<Airship::Pipeline>(vertexShader, fragmentShader,
                                                     std::vector<Airship::Pipeline::VertexAttributeDesc>{
                                                         {"Position", 0, Airship::ShaderDataType::Float2},
                                                         {"Color", 1, Airship::ShaderDataType::Float4},
                                                     });
    flatShadedMaterial = std::make_unique<Airship::Material>(m_Pipeline.get());
    // BG coloring pipeline - stored in text files
    auto bgVertShader = Airship::Shader::from_file(Airship::ShaderType::Vertex, "assets/grass.vert");
    auto bgFragShader = Airship::Shader::from_file(Airship::ShaderType::Fragment, "assets/grass.frag");
    m_BGPipeline = std::make_unique<Airship::Pipeline>(
        bgVertShader, bgFragShader,
        std::vector<Airship::Pipeline::VertexAttributeDesc>{{"Position", 0, Airship::ShaderDataType::Float2}});
    backgroundMaterial = std::make_unique<Airship::Material>(m_BGPipeline.get());
}

void Game::OnKeyPress(const Airship::Window& window, Airship::Input::Key key, int scancode,
                      Airship::Input::KeyAction action, Airship::Input::KeyMods mods) {
    UNUSED(window);
    UNUSED(scancode);
    UNUSED(mods);
    if (action != Airship::Input::KeyAction::Press) return;

    switch (key) {
    case Airship::Input::Key::W:
    case Airship::Input::Key::Up: {
        if (m_Snake.GetDir() != Direction::Down) m_Snake.SetDir(Direction::Up);
        return;
    }
    case Airship::Input::Key::A:
    case Airship::Input::Key::Left: {
        if (m_Snake.GetDir() != Direction::Right) m_Snake.SetDir(Direction::Left);
        return;
    }
    case Airship::Input::Key::S:
    case Airship::Input::Key::Down: {
        if (m_Snake.GetDir() != Direction::Up) m_Snake.SetDir(Direction::Down);
        return;
    }
    case Airship::Input::Key::D:
    case Airship::Input::Key::Right: {
        if (m_Snake.GetDir() != Direction::Left) m_Snake.SetDir(Direction::Right);
        return;
    }
    case Airship::Input::Key::Space: {
        m_Snake.Grow();
        return;
    }
    default:
        return;
    }
}

void Game::OnStart() {
    SHIPLOG_INFO("Starting");
    CreatePipelines();

    // Gridlines on the dual grid
    Grid dualGrid(GRID_SIZE, vec2{GRID_UL, GRID_UL}, {GRID_DIMS + 1, GRID_DIMS + 1});
    OwningStream<vec2>& positionStream = m_GridMesh.addStream<vec2>("Position", Airship::ShaderDataType::Float2);
    std::vector<vec2>& positions = positionStream.data();
    OwningStream<Airship::Color>& colorStream =
        m_GridMesh.addStream<Airship::Color>("Color", Airship::ShaderDataType::Float4);
    std::vector<Airship::Color>& colors = colorStream.data();

    for (int i = 0; i < dualGrid.GetBounds().y(); i++) {
        vec2 start = dualGrid.pos({0, i});
        start = start - vec2(1.0f, 1.0f) * LINE_WIDTH / 2;
        vec2 end = dualGrid.pos({dualGrid.GetBounds().x() - 1, i});
        end = end + vec2(1.0f, 1.0f) * LINE_WIDTH / 2;
        float hLineLength = end.x() - start.x();

        for (auto squarePt : canonicalSquare) {
            vec2 pt = vec2(float(squarePt.x()) * hLineLength, float(squarePt.y()) * LINE_WIDTH) + start;
            positions.emplace_back(pt);
            colors.emplace_back(1.0f, 1.0f, 1.0f, 0.2f);
            m_GridMesh.setVertexCount(m_GridMesh.vertexCount() + 1);
        }
    }

    for (int i = 0; i < dualGrid.GetBounds().x(); i++) {
        vec2 start = dualGrid.pos({i, 0});
        start = start - vec2(1.0f, 1.0f) * LINE_WIDTH / 2;
        vec2 end = dualGrid.pos({i, dualGrid.GetBounds().y() - 1});
        end = end + vec2(1.0f, 1.0f) * LINE_WIDTH / 2;
        float vLineLength = end.y() - start.y();

        for (auto squarePt : canonicalSquare) {
            vec2 pt = vec2(float(squarePt.x()) * LINE_WIDTH, float(squarePt.y()) * vLineLength) + start;
            positions.emplace_back(pt);
            colors.emplace_back(1.0f, 1.0f, 1.0f, 0.5f);
            m_GridMesh.setVertexCount(m_GridMesh.vertexCount() + 1);
        }
    }
    positionStream.invalidate();
    colorStream.invalidate();

    m_Grid.SetOOBCallback([](ivec2 bounds, ivec2 pos) -> ivec2 {
        // NOLINTNEXTLINE(bugprone-lambda-function-name)
        SHIPLOG_ALERT("Handling OOB position\n");
        pos[0] = (pos[0] + bounds[0]) % bounds[0];
        pos[1] = (pos[1] + bounds[1]) % bounds[1];
        return pos;
    });

    // BG mesh: just has positions, composed of a square across the background.
    auto& bgstream = m_BGMesh.addStream<vec2>("Position", Airship::ShaderDataType::Float2);
    auto& bgpos = bgstream.data();
    for (const auto& pt : canonicalSquare) {
        bgpos.emplace_back(pt * 2 - vec2(1.0f, 1.0f));
    }
    bgstream.invalidate();
    m_BGMesh.setVertexCount(canonicalSquare.size());

    m_Snake.Initialize({5, 5});
    ivec2 applePos;
    applePos.x() = static_cast<int>(randomRange(0, static_cast<float>(m_Grid.GetBounds().x())));
    applePos.y() = static_cast<int>(randomRange(0, static_cast<float>(m_Grid.GetBounds().y())));
    m_Apple = std::make_unique<Apple>(applePos, &m_Grid, Airship::Colors::Red);

    backgroundMaterial->SetUniform("iTip", m_BladeTipColor);
    backgroundMaterial->SetUniform("iMaxTipDeviation", 1.0f);
}

void Game::OnGameLoop(float elapsed) {
    static const auto start_time = std::chrono::steady_clock::now();
    const auto cur_time = std::chrono::steady_clock::now();
    std::chrono::duration<float> duration = cur_time - start_time;
    backgroundMaterial->SetUniform("iTime", duration.count());
    if (!m_Snake.IsAlive()) {
        constexpr Airship::Color deathColor = {0.2f, 0.0f, 0.0f};
        m_BladeTipColor = Airship::Color::lerp(m_BladeTipColor, deathColor, 0.05f);
        backgroundMaterial->SetUniform("iTip", m_BladeTipColor);
        draw();
        return;
    }
    m_TickTime += elapsed;
    if (m_TickTime < FRAME_TIME) {
        draw();
        return;
    }
    m_TickTime = 0;
    SHIPLOG_DEBUG("Loop");
    m_Snake.Update(m_Apple->pos());
    if (!m_Snake.IsAlive()) {
        backgroundMaterial->SetUniform("iMaxTipDeviation", 0.0f);
    }
    if (m_Snake.HeadPos() == m_Apple->pos()) {
        ivec2 applePos;
        applePos.x() = static_cast<int>(randomRange(0, static_cast<float>(m_Grid.GetBounds().x())));
        applePos.y() = static_cast<int>(randomRange(0, static_cast<float>(m_Grid.GetBounds().y())));
        m_Apple->SetPos(applePos);
        m_Tallies.increment();
    }
    draw();
}
