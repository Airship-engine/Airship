#include "core/application.h"

#include "gtest/gtest.h"
#include "spdlog/spdlog.h"

namespace Airship
{
namespace Test
{

class AirshipTestEnvironment : public ::testing::Environment 
{
public:
    ~AirshipTestEnvironment() override {}

    void SetUp() override 
    {
		Airship::ShipLog::get().AddListener("Test Game Log", [](Airship::ShipLog::Level level, std::string_view msg) {
			// If we ever log an error, this will cause the test to fail
			FAIL();
		}, Airship::ShipLog::Level::ERROR);
    }
};

// Initialize the airship test environment
#if !defined(DISABLE_TEST_ENVIRONMENT)
extern const ::testing::Environment* airship_environment;
#endif

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

} // namespace Test
} // namespace Airship
