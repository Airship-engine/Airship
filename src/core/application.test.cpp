#include "core/application.h"
#include "gtest/gtest.h"
#include "test/common.h"

TEST(Window, nonnull) {
    Airship::Test::GameClass app;
    app.Run();

    EXPECT_EQ(app.GetWindow().has_value(), true);
}
TEST(Window, null) {
    Airship::Test::GameClass app(-1, -1);
    app.Run();

    EXPECT_EQ(app.GetWindow().has_value(), false);
}
