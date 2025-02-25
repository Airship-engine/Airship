#pragma once

constexpr float SMALL_ALPHA = 1.0e-6f;

namespace Airship {

struct Color {

    constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    bool operator==(const Color &other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    // https://en.wikipedia.org/wiki/Blend_modes
    enum class BlendMode {
        Alpha, // Same as over compositing
        Multiply,
        Add,
        Average
    };
    static Color blend(const Color &bg, const Color &fg, BlendMode mode = BlendMode::Alpha);
    static Color lerp(const Color &bg, const Color &fg, float t);

    // https://en.wikipedia.org/wiki/Alpha_compositing
    enum class CompositeMode {
        Over, // Same as alpha blending
    };
    static Color composite(const Color &bg, const Color &fg, CompositeMode mode = CompositeMode::Over);

    enum class NormalizeMode {
        Clip,
        Scale
    };
    Color normalize(NormalizeMode mode = NormalizeMode::Scale);

    static const Color White;   
    static const Color Black;

    static const Color LightGrey;
    static const Color Grey;
    static const Color DarkGrey;
    
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    
    static const Color Cyan;
    static const Color Magenta;
    static const Color Yellow;
    
    static const Color CornflowerBlue;
    static const Color Orange;

    float r, g, b, a;
    
    static Color HSV(float h, float s, float v, float a = 1.0f);
    float H() const;
    float S() const;
    float V() const;
};


} // namespace Airship
