
#include "core/logging.h"
#include "render/color.h"

#include "gtest/gtest.h"

TEST(Color, SimpleBlend) {
    Airship::Color red = Airship::Color::Red;
    Airship::Color green = Airship::Color::Green;
    Airship::Color blue = Airship::Color::Blue;

    EXPECT_EQ(
        Airship::Color::blend(red, green, Airship::Color::BlendMode::Add),
        Airship::Color::Yellow
    );
    EXPECT_EQ(
        Airship::Color::blend(red, blue, Airship::Color::BlendMode::Add),
        Airship::Color::Magenta
    );
    EXPECT_EQ(
        Airship::Color::blend(blue, green, Airship::Color::BlendMode::Add),
        Airship::Color::Cyan
    );

    Airship::Color cyan = Airship::Color::Cyan;
    Airship::Color magenta = Airship::Color::Magenta;
    Airship::Color yellow = Airship::Color::Yellow;
    EXPECT_EQ(
        Airship::Color::blend(red, cyan, Airship::Color::BlendMode::Add),
        Airship::Color::White
    );
    EXPECT_EQ(
        Airship::Color::blend(green, magenta, Airship::Color::BlendMode::Add),
        Airship::Color::White
    );
    EXPECT_EQ(
        Airship::Color::blend(blue, yellow, Airship::Color::BlendMode::Add),
        Airship::Color::White
    );

    Airship::Color white = Airship::Color::White;
    Airship::Color grey = Airship::Color::Grey;
    Airship::Color black = Airship::Color::Black;
    EXPECT_EQ(
        Airship::Color::blend(white, black, Airship::Color::BlendMode::Add),
        white
    );
    EXPECT_EQ(
        Airship::Color::blend(white, black, Airship::Color::BlendMode::Average),
        grey
    );
}

TEST(Color, Normalize) {
    Airship::Color white = Airship::Color::White;
    Airship::Color red = Airship::Color::Red;
    Airship::Color magenta = Airship::Color::Magenta;

    EXPECT_EQ(
        white.normalize(),
        white
    );
    Airship::Color extraRed = Airship::Color::blend(red, magenta, Airship::Color::BlendMode::Add);
    EXPECT_NE(
        extraRed,
        magenta
    );
    EXPECT_EQ(
        extraRed.normalize(Airship::Color::NormalizeMode::Clip),
        magenta
    );

    Airship::Color green = Airship::Color::Green;
    Airship::Color tooOrange = Airship::Color::blend(red, red, Airship::Color::BlendMode::Add);
    tooOrange = Airship::Color::blend(tooOrange, green, Airship::Color::BlendMode::Add);
    Airship::Color orange = Airship::Color::Orange;
    EXPECT_NE(
        tooOrange,
        orange
    );
    EXPECT_EQ(
        tooOrange.normalize(),
        orange
    );
}

TEST(Color, HSV) {
    auto expect_hsv = [](Airship::Color color, float h, float s, float v) {
        EXPECT_EQ(
            color.H(),
            h
        );
        EXPECT_EQ(
            color.S(),
            s
        );
        EXPECT_EQ(
            color.V(),
            v
        );
    };

    expect_hsv(Airship::Color::Red, 0.0f, 1.0f, 1.0f);
    expect_hsv(Airship::Color::Green, 120.0f, 1.0f, 1.0f);
    expect_hsv(Airship::Color::Blue, 240.0f, 1.0f, 1.0f);

    expect_hsv(Airship::Color::Black, 0.0f, 0.0f, 0.0f);
    expect_hsv(Airship::Color::Grey, 0.0f, 0.0f, 0.5f);
    expect_hsv(Airship::Color::White, 0.0f, 0.0f, 1.0f);

    auto expect_rgb = [](Airship::Color color, float r, float g, float b) {
        EXPECT_NEAR(
            color.r,
            r,
            1e-6f
        );
        EXPECT_NEAR(
            color.g,
            g,
            1e-6f
        );
        EXPECT_NEAR(
            color.b,
            b,
            1e-6f
        );
    };

    expect_rgb(Airship::Color::HSV(0.0f, 1.0f, 1.0f), 1.0f, 0.0f, 0.0f);
    expect_rgb(Airship::Color::HSV(120.0f, 1.0f, 1.0f), 0.0f, 1.0f, 0.0f);
    expect_rgb(Airship::Color::HSV(240.0f, 1.0f, 1.0f), 0.0f, 0.0f, 1.0f);

    // RGB -> HSV -> RGB without error
    for (float r = 0; r <= 1; r += 0.2) {
        for (float g = 0; g <= 1; g += 0.2) {
            for (float b = 0; b <= 1; b += 0.2) {
                Airship::Color color(r,g,b);
                float h = color.H();
                float s = color.S();
                float v = color.V();
                Airship::Color converted = Airship::Color::HSV(h,s,v);
                SHIPLOG_INFO("r/g/b={}/{}/{}", r, g, b);
                SHIPLOG_INFO("h/s/v={}/{}/{}", h, s, v);
                SHIPLOG_INFO("--> r/g/b={}/{}/{}", converted.r, converted.g, converted.b);
                expect_rgb(
                    Airship::Color::HSV(h,s,v),
                    color.r, color.g, color.b
                );
            }   
        }
    }
}