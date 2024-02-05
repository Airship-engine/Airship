#include "core/application.h"
#include "spdlog/spdlog.h"

class GameClass : public Airship::Application
{
public:
    GameClass() = default;
    GameClass(int height, int width) : height(height), width(width) {}
    const std::optional<Airship::Window*> GetWindow() const { return win; }
protected:
    void OnStart() override
    {
        win = CreateWindow(width, height);
    }

    int height = 800, width = 600;
    std::optional<Airship::Window*> win;
};
