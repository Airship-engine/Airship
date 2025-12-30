#include <optional>
#include <string_view>

#include "core/application.h"
#include "core/logging.h"
#include "core/window.h"
#include "gtest/gtest.h"

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
    GameClass() = default;
    GameClass(int height, int width) : height(height), width(width) {}
    [[nodiscard]] std::optional<Airship::Window*> GetWindow() const { return win; }

protected:
    void OnStart() override;

    int height = 800, width = 600;
    std::optional<Airship::Window*> win;
};

} // namespace Airship::Test

