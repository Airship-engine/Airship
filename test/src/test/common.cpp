#include "test/common.h"

namespace Airship
{
namespace Test
{

#if !defined(DISABLE_TEST_ENVIRONMENT)
const ::testing::Environment* airship_environment = ::testing::AddGlobalTestEnvironment(new AirshipTestEnvironment);
#endif

void Airship::Test::GameClass::OnStart()
{
	win = CreateWindow(width, height, "test app");
}

} // namespace Test
} // namespace Airship
