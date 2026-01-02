#include "core/application.h"
#include "core/window.h"
#include "render/opengl/renderer.h"

class Game : public Airship::Application {
public:
    Game() = default;
    Game(int height, int width) : m_Height(height), m_Width(width) {}

    void OnStart() override;

protected:
    int m_Height = 800, m_Width = 600;
    Airship::Window* m_MainWin = nullptr;
    Airship::Renderer m_Renderer;
};
