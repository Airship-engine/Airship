#include "core/logging.h"
#include "gtest/gtest.h"

TEST(Logging, coutinfo)
{
    Airship::Logging::Init();
    EXPECT_NE(nullptr, Airship::Logging::g_ApplicationLogger);
}
