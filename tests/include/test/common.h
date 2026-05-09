#include <string_view>

#include "color.h"
#include "core/application.h"
#include "core/logging.h"
#include "core/window.h"
#include "gtest/gtest.h"
#include "opengl/renderer.h"

namespace Airship::Test {

class AirshipTestEnvironment : public ::testing::Environment {
public:
    ~AirshipTestEnvironment() override = default;

    void SetUp() override {
        Airship::ShipLog::get().AddListener(
            "Test Game Log",
            [](Airship::ShipLog::Level /*level*/, std::string_view /*msg*/) {
                // If we ever log an error, this will cause the test to fail
                FAIL();
            },
            Airship::ShipLog::Level::ERROR);
    }
};

// Initialize the airship test environment
#ifndef DISABLE_TEST_ENVIRONMENT
// NOLINTNEXTLINE // non-const global variable
extern const ::testing::Environment* airship_environment;
#endif

class GameClass : public Airship::Application {
public:
    GameClass(bool servermode) : Airship::Application(servermode) {}
    GameClass(int width, int height) : Airship::Application(width, height, "test app") {}
    GameClass() : GameClass(600, 800) {}
    [[nodiscard]] Airship::Window* GetWindow() const { return m_MainWindow.get(); }
    [[nodiscard]] const Airship::Renderer& GetRenderer() const { return m_Renderer; }

protected:
    void OnStart() override { m_Renderer.setClearColor(Airship::Colors::CornflowerBlue); }
    void OnGameLoop(float /*elapsed*/) override {
        m_ShouldClose = true; // Skip the game loop
    }
};

} // namespace Airship::Test

