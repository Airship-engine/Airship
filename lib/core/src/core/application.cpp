#include "core/application.h"

#include <cassert>
#include <memory>
#include <string>
#include <utility>

#include "core/window.h"
#include "logging.h"

namespace Airship {

Application::Application(bool servermode) : m_ServerMode(servermode) {};

Application::Application(int width, int height, std::string title) :
    m_Width(width), m_Height(height), m_Title(std::move(title)) {}

void Application::Run() {
    if (!m_ServerMode) {
        Window::Init();
        if (m_Width < 0 || m_Height < 0) SHIPLOG_ERROR("Creating a window with negative dimensions");
        m_MainWindow = std::make_unique<Window>(m_Width, m_Height, m_Title, true);
    }
    OnStart();
}

Application::~Application() {
    if (!m_ServerMode) {
        m_MainWindow.reset();
        Window::Terminate();
    }
}

} // namespace Airship
