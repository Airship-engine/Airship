#include "core/application.h"
#include "GLFW/glfw3.h"

#include <cassert>
#include <iostream>
#include <string>

namespace Airship {

void Application::Run() {
    GLFW_CHECK(glfwInit());
    OnStart();
}

Application::~Application() {
    GLFW_CHECK(glfwTerminate());
}

std::optional<Window*> Application::CreateWindow(int w, int h, const std::string &title, bool visible)
{
    if (w < 1 || h < 1) return std::nullopt;
    return m_Windows.emplace_back(std::make_unique<Window>(w, h, title, visible)).get();
}

} // namespace Airship
