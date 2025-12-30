#include "test/common.h"
#include <gtest/gtest.h>


namespace Airship::Test
{

#ifndef DISABLE_TEST_ENVIRONMENT
//NOLINTNEXTLINE // non-const global variable
const ::testing::Environment* airship_environment = ::testing::AddGlobalTestEnvironment(new AirshipTestEnvironment);
#endif

void Airship::Test::GameClass::OnStart()
{
    win = CreateWindow(width, height, "test app");
}

} // namespace Airship::Test

