#include "core/application.h"
#include "gtest/gtest.h"

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

TEST(Window, nonnull) {
    GameClass app;
    app.Run();

    EXPECT_EQ(app.GetWindow().has_value(), true);
}
TEST(Window, null) {
    GameClass app(-1, -1);
    app.Run();

    EXPECT_EQ(app.GetWindow().has_value(), false);
}