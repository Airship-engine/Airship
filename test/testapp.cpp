#include "core/application.h"
#include "gtest/gtest.h"
#include "common.h"

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