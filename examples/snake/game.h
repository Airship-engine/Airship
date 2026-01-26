#include <cassert>
#include <limits>
#include <memory>

#include "addons.h"
#include "color.h"
#include "core/application.h"
#include "grid.h"
#include "input.h"
#include "logging.h"
#include "opengl/renderer.h"
#include "snake.h"
#include "tallies.h"
#include "window.h"

constexpr float GRID_DIMS = 50;
constexpr float GRID_PADDING = 0.1f;

constexpr float GRID_WIDTH = 2 * (1.0f - GRID_PADDING);
constexpr float GRID_SIZE = GRID_WIDTH / GRID_DIMS;
constexpr float GRID_UL = -1.0f + GRID_PADDING;

class Game : public Airship::Application {
public:
    Game() :
        // Airship::Application(true),
        Airship::Application(800, 800, "Snake Example"),
        m_Grid(GRID_SIZE, vec2{GRID_UL, GRID_UL}, {GRID_DIMS, GRID_DIMS}), m_Snake(&m_Grid) {
        Airship::ShipLog::get().SetLevel("default_log", Airship::ShipLog::Level::TRACE);
    }

    void OnStart() override;
    void OnGameLoop(float elapsed) override;
    void OnKeyPress(const Airship::Window& window, Airship::Input::Key key, int scancode,
                    Airship::Input::KeyAction action, Airship::Input::KeyMods mods) override;

private:
    void draw() {
        if (m_Renderer) {
            m_Renderer->clear();
            m_BGMesh.draw(*m_Renderer, m_BGPipeline.get());
            m_GridMesh.draw(*m_Renderer, m_Pipeline.get());
            m_Snake.draw(*m_Renderer, m_Pipeline.get());
            m_Apple->draw(*m_Renderer, m_Pipeline.get());
            m_Tallies.draw(*m_Renderer, *m_Pipeline);
        }
    }
    void CreatePipelines();
    std::unique_ptr<Airship::Pipeline> m_Pipeline;
    std::unique_ptr<Airship::Pipeline> m_BGPipeline;
    float m_TickTime = std::numeric_limits<float>::max();
    Grid<2> m_Grid;
    std::unique_ptr<Apple> m_Apple;
    Snake m_Snake;
    DynamicMesh m_GridMesh;
    DynamicMesh m_BGMesh;
    // Let's use a darker green than what comes with Airship
    Airship::Color m_BladeTipColor = {0.04f, 0.07f, 0.0f};
    Tallies m_Tallies = {vec2(-0.95f, -0.97f), vec2(0.1f, 0.06f)};
};
