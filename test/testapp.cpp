#include "core/application.h"

#define EXPECT(expr, val) if ((expr) != val) return EXIT_FAILURE

class GameClass : public Airship::Application
{
public:
    GameClass() = default;
    GameClass(int height, int width) : height(height), width(width) {}
    const std::optional<Airship::Window*> GetWindow() const { return win; }
protected:
    void OnStart() override;
    int height = 800, width = 600;
    std::optional<Airship::Window*> win;
};

void GameClass::OnStart()
{
    win = CreateWindow(width, height);
}

int main()
{ 
    GameClass app;
    app.Run();

    EXPECT(app.GetWindow().has_value(), true);

    GameClass app2(-1, -1);
    app2.Run();

    EXPECT(app2.GetWindow().has_value(), false);
}
