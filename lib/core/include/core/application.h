#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "core/window.h"

namespace Airship {
class Application {
public:
    void Run();
    virtual ~Application();

protected:
    std::optional<Window*> CreateWindow(int w, int h, const std::string& title, bool visible = true);

    virtual void OnStart() {}

private:
    std::vector<std::unique_ptr<Window>> m_Windows;
};
} // namespace Airship
