#include "core/application.h"

#include <cassert>
#include <iostream>

namespace Airship {

void Application::Run() {
    OnStart();
}

std::optional<Window*> Application::CreateWindow(int w, int h)
{
    if (w < 1 || h < 1) return std::nullopt;
    return m_Windows.emplace_back(std::make_unique<Window>(w, h)).get();
}

} // namespace Airship
