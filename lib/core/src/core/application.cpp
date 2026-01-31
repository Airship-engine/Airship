#include "core/application.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "core/input.h"
#include "core/instrumentation.h"
#include "core/logging.h"
#include "core/window.h"
#include "opengl/renderer.h"

namespace Airship {

Application::Application(bool servermode) : m_ServerMode(servermode) {
    assert(m_ServerMode);
}

Application::Application(int width, int height, std::string title) :
    m_Width(width), m_Height(height), m_Title(std::move(title)) {}

void Application::Run() {
    if (!m_ServerMode) {
        Window::Init();
        if (m_Width < 0 || m_Height < 0) SHIPLOG_ERROR("Creating a window with negative dimensions");
        m_MainWindow = std::make_unique<Window>(m_Width, m_Height, m_Title, true);
        m_MainWindow->setWindowResizeCallback([this](int width, int height) {
            m_Renderer->resize(width, height);
            m_Height = height;
            m_Width = width;
        });
        m_MainWindow->setKeyPressCallback(
            [this](const Window& window, Input::Key key, int scancode, Input::KeyAction action, Input::KeyMods mods) {
                OnKeyPress(window, key, scancode, action, mods);
            });

        m_Renderer = std::make_unique<Renderer>();
        m_Renderer->init();
        m_Renderer->resize(m_Width, m_Height);
    }
    OnStart();
    GameLoop();
    Profiling::dump("temp_file");
}

void Application::GameLoop() {
    PROFILE_FUNCTION();
    auto startTime = std::chrono::system_clock::now();
    while (!m_ShouldClose) {
        PROFILE_SCOPE("frame");
        if (m_MainWindow) m_MainWindow->pollEvents();
        auto frameTime = std::chrono::system_clock::now() - startTime;
        startTime = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration<float>(frameTime).count();
        elapsed = std::min(elapsed, 0.1f); // Clamp to avoid large jumps

        {
            PROFILE_SCOPE("User game loop");
            OnGameLoop(elapsed);
        }

        // Show the rendered buffer
        if (m_MainWindow) {
            m_MainWindow->swapBuffers();
            m_ShouldClose |= m_MainWindow->shouldClose();
        }
    }
}

Application::~Application() {
    if (!m_ServerMode) {
        m_MainWindow.reset();
        Window::Terminate();
    }
}

} // namespace Airship
