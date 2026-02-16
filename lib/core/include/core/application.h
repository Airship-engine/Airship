#pragma once

#include <memory>
#include <string>

#include "core/input.h"
#include "core/window.h"
#include "render/opengl/renderer.h"

namespace Airship {
class Application {
public:
    Application() = default;
    Application(bool serverMode);
    Application(int width, int height, std::string title);
    void Run();
    virtual ~Application();

protected:
    // User-facing hooks
    virtual void OnStart() {}
    virtual void OnGameLoop(float /*dt*/) {}
    // For keystroke handling, not text
    virtual void OnKeyPress(const Window& /*window*/, Input::Key /*key*/, int /*scancode*/, Input::KeyAction /*action*/,
                            Input::KeyMods /*mods*/) {}

    bool m_ShouldClose = false;
    Renderer m_Renderer;

    std::unique_ptr<Window> m_MainWindow;
    int m_Width = 800, m_Height = 600;

private:
    void GameLoop();
    std::string m_Title;
    bool m_ServerMode = false;
};
} // namespace Airship
