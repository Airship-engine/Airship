#pragma once

namespace Airship {

struct HSVColor;

struct RGBColor {

    constexpr RGBColor(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    RGBColor(const HSVColor& other);

    bool operator==(const RGBColor& other) const = default;

    // https://en.wikipedia.org/wiki/Blend_modes
    enum class BlendMode {
        Alpha, // Same as over compositing
        Multiply,
        Add,
        Average
    };
    static RGBColor blend(const RGBColor& bg, const RGBColor& fg, BlendMode mode = BlendMode::Alpha);
    static RGBColor lerp(const RGBColor& bg, const RGBColor& fg, float t);

    enum class NormalizeMode {
        Clamp,
        Scale
    };
    RGBColor normalize(NormalizeMode mode = NormalizeMode::Scale);

    float r, g, b, a;
};
using Color = RGBColor;

struct HSVColor {
    HSVColor(float h, float s, float v, float a = 1.0f) : h(h), s(s), v(v), a(a) {}
    HSVColor(const RGBColor& other);

    float h, s, v, a;
};

namespace Colors {

constexpr RGBColor White(1.0f, 1.0f, 1.0f);
constexpr RGBColor Black(0.0f, 0.0f, 0.0f);

constexpr RGBColor LightGrey(0.75f, 0.75f, 0.75f);
constexpr RGBColor Grey(0.5f, 0.5f, 0.5f);
constexpr RGBColor DarkGrey(0.25f, 0.25f, 0.25f);

constexpr RGBColor Red(1.0f, 0.0f, 0.0f);
constexpr RGBColor Green(0.0f, 1.0f, 0.0f);
constexpr RGBColor Blue(0.0f, 0.0f, 1.0f);

constexpr RGBColor Cyan(0.0f, 1.0f, 1.0f);
constexpr RGBColor Magenta(1.0f, 0.0f, 1.0f);
constexpr RGBColor Yellow(1.0f, 1.0f, 0.0f);

constexpr RGBColor Orange(1.0f, 0.5f, 0.0f);

constexpr RGBColor CornflowerBlue(0.39f, 0.582f, 0.926f);

} // namespace Colors
} // namespace Airship
