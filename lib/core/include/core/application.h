#pragma once

#include <memory>
#include <string>

#include "core/window.h"

namespace Airship {
class Application {
public:
    Application() = default;
    Application(bool serverMode);
    Application(int width, int height, std::string title);
    void Run();
    virtual ~Application();

protected:
    virtual void OnStart() {}

    std::unique_ptr<Window> m_MainWindow;
    int m_Width = 800, m_Height = 600;

private:
    std::string m_Title;
    bool m_ServerMode = false;
};
} // namespace Airship
