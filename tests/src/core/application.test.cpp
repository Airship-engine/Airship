#include "gtest/gtest.h"
#include "test/common.h"

TEST(Window, nonnull) {
    Airship::Test::GameClass app;
    app.Run();

    EXPECT_NE(app.GetWindow(), nullptr);
}
TEST(Window, null) {
    Airship::Test::GameClass app(-1, -1);
    EXPECT_DEATH(app.Run(), "");

    Airship::Test::GameClass app2(true);
    app2.Run();

    EXPECT_EQ(app2.GetWindow(), nullptr);
}
