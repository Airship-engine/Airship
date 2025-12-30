
#include "render/color.h"

#include "core/logging.h"
#include "gtest/gtest.h"

TEST(Color, SimpleBlend) {
    const Airship::Color red = Airship::Colors::Red;
    const Airship::Color green = Airship::Colors::Green;
    const Airship::Color blue = Airship::Colors::Blue;

    EXPECT_EQ(Airship::Color::blend(red, green, Airship::Color::BlendMode::Add), Airship::Colors::Yellow);
    EXPECT_EQ(Airship::Color::blend(red, blue, Airship::Color::BlendMode::Add), Airship::Colors::Magenta);
    EXPECT_EQ(Airship::Color::blend(blue, green, Airship::Color::BlendMode::Add), Airship::Colors::Cyan);

    const Airship::Color cyan = Airship::Colors::Cyan;
    const Airship::Color magenta = Airship::Colors::Magenta;
    const Airship::Color yellow = Airship::Colors::Yellow;
    EXPECT_EQ(Airship::Color::blend(red, cyan, Airship::Color::BlendMode::Add), Airship::Colors::White);
    EXPECT_EQ(Airship::Color::blend(green, magenta, Airship::Color::BlendMode::Add), Airship::Colors::White);
    EXPECT_EQ(Airship::Color::blend(blue, yellow, Airship::Color::BlendMode::Add), Airship::Colors::White);

    const Airship::Color white = Airship::Colors::White;
    const Airship::Color grey = Airship::Colors::Grey;
    const Airship::Color black = Airship::Colors::Black;
    EXPECT_EQ(Airship::Color::blend(white, black, Airship::Color::BlendMode::Add), white);
    EXPECT_EQ(Airship::Color::blend(white, black, Airship::Color::BlendMode::Average), grey);
}

TEST(Color, Normalize) {
    const Airship::Color white = Airship::Colors::White;
    const Airship::Color red = Airship::Colors::Red;
    const Airship::Color magenta = Airship::Colors::Magenta;

    EXPECT_EQ(white.normalize(), white);
    const Airship::Color extraRed = Airship::Color::blend(red, magenta, Airship::Color::BlendMode::Add);
    EXPECT_NE(extraRed, magenta);
    EXPECT_EQ(extraRed.normalize(Airship::Color::NormalizeMode::Clamp), magenta);

    const Airship::Color green = Airship::Colors::Green;
    Airship::Color tooOrange = Airship::Color::blend(red, red, Airship::Color::BlendMode::Add);
    tooOrange = Airship::Color::blend(tooOrange, green, Airship::Color::BlendMode::Add);
    const Airship::Color orange = Airship::Colors::Orange;
    EXPECT_NE(tooOrange, orange);
    EXPECT_EQ(tooOrange.normalize(), orange);
}

TEST(Color, HSV) {
    auto expect_hsv = [](Airship::HSVColor hsv, float h, float s, float v) {
        EXPECT_EQ(hsv.h, h);
        EXPECT_EQ(hsv.s, s);
        EXPECT_EQ(hsv.v, v);
    };

    expect_hsv(Airship::Colors::Red, 0.0f, 1.0f, 1.0f);
    expect_hsv(Airship::Colors::Green, 120.0f, 1.0f, 1.0f);
    expect_hsv(Airship::Colors::Blue, 240.0f, 1.0f, 1.0f);

    expect_hsv(Airship::Colors::Black, 0.0f, 0.0f, 0.0f);
    expect_hsv(Airship::Colors::Grey, 0.0f, 0.0f, 0.5f);
    expect_hsv(Airship::Colors::White, 0.0f, 0.0f, 1.0f);

    auto expect_rgb = [](Airship::RGBColor rgb, float r, float g, float b) {
        EXPECT_NEAR(rgb.r, r, 1e-6f);
        EXPECT_NEAR(rgb.g, g, 1e-6f);
        EXPECT_NEAR(rgb.b, b, 1e-6f);
    };

    expect_rgb(Airship::HSVColor(0.0f, 1.0f, 1.0f), 1.0f, 0.0f, 0.0f);
    expect_rgb(Airship::HSVColor(120.0f, 1.0f, 1.0f), 0.0f, 1.0f, 0.0f);
    expect_rgb(Airship::HSVColor(240.0f, 1.0f, 1.0f), 0.0f, 0.0f, 1.0f);

    // RGB -> HSV -> RGB without error
    for (float r = 0; r <= 1; r += 0.2) {
        for (float g = 0; g <= 1; g += 0.2) {
            for (float b = 0; b <= 1; b += 0.2) {
                Airship::Color rgb(r, g, b);
                Airship::HSVColor hsv(rgb);
                Airship::Color converted(hsv);
                SHIPLOG_INFO("r/g/b={}/{}/{}", r, g, b);
                SHIPLOG_INFO("h/s/v={}/{}/{}", hsv.h, hsv.s, hsv.v);
                SHIPLOG_INFO("--> r/g/b={}/{}/{}", converted.r, converted.g, converted.b);
                expect_rgb(hsv, rgb.r, rgb.g, rgb.b);
            }
        }
    }
}
