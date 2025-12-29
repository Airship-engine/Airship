
#include "render/color.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef NDEBUG
#define ERR_COLOR Airship::Colors::Black
#else
#define ERR_COLOR Airship::Colors::Green
#endif

namespace Airship {
namespace {
    constexpr float SMALL_ALPHA = 1.0e-6f;

    RGBColor blendAlpha(const RGBColor &bg, const RGBColor &fg) {
        const float newA = fg.a + (bg.a * (1 - fg.a));
        if (newA < SMALL_ALPHA) {
            return {0.0f, 0.0f, 0.0f, 0.0f};
        }
        const float bgScale = bg.a * (1 - fg.a) / newA;
        const float fgScale = fg.a / newA;
        return {
            (fg.r * fgScale) + (bg.r * bgScale),
            (fg.g * fgScale) + (bg.g * bgScale),
            (fg.b * fgScale) + (bg.b * bgScale),
            newA
        };
    }

    RGBColor blendMultiply(const RGBColor &bg, const RGBColor &fg) {
        const float newR = bg.r * fg.r;
        const float newG = bg.g * fg.g;
        const float newB = bg.b * fg.b;
        const float newA = bg.a * fg.a;
        return {newR, newG, newB, newA};
    }

    RGBColor blendAdd(const RGBColor &bg, const RGBColor &fg) {
        const float newR = bg.r + fg.r;
        const float newG = bg.g + fg.g;
        const float newB = bg.b + fg.b;
        const float newA = std::max(bg.a, fg.a);
        return {newR, newG, newB, newA};
    }

    RGBColor blendAverage(const RGBColor &bg, const RGBColor &fg) {
        const float newR = (bg.r + fg.r) / 2;
        const float newG = (bg.g + fg.g) / 2;
        const float newB = (bg.b + fg.b) / 2;
        const float newA = std::max(bg.a, fg.a);
        return {newR, newG, newB, newA};
    }
}

RGBColor RGBColor::blend(const RGBColor &bg, const RGBColor &fg, RGBColor::BlendMode mode) {
    switch (mode) {
        case RGBColor::BlendMode::Alpha: return blendAlpha(bg, fg);
        case RGBColor::BlendMode::Multiply: return blendMultiply(bg, fg);
        case RGBColor::BlendMode::Add: return blendAdd(bg, fg);
        case RGBColor::BlendMode::Average: return blendAverage(bg, fg);
    }
    assert(false);
    return ERR_COLOR;
}

RGBColor RGBColor::lerp(const RGBColor &bg, const RGBColor &fg, float t) {
    const RGBColor A(fg.r * t, fg.g * t, fg.b * t, fg.a * t);
    const RGBColor B(bg.r * (1 - t), bg.g * (1 - t), bg.b * (1 - t), bg.a * (1 - t));
    return blend(A, B, RGBColor::BlendMode::Add);
}

namespace {
    RGBColor normalizeClamp(const RGBColor &color) {
        const float newR = std::clamp(color.r, 0.0f, 1.0f);
        const float newG = std::clamp(color.g, 0.0f, 1.0f);
        const float newB = std::clamp(color.b, 0.0f, 1.0f);
        const float newA = std::clamp(color.a, 0.0f, 1.0f);
        return {newR, newG, newB, newA};
    }
    
    RGBColor normalizeScale(const RGBColor &color) {
        float div = std::max(color.r, color.g);
        div = std::max(div, color.b);
        const float newR = color.r / div;
        const float newG = color.g / div;
        const float newB = color.b / div;
        const float newA = std::clamp(color.r, 0.0f, 1.0f);
        return {newR, newG, newB, newA};
    }
}

RGBColor RGBColor::normalize(RGBColor::NormalizeMode mode) const {
    switch (mode) {
        case RGBColor::NormalizeMode::Clamp: return normalizeClamp(*this);
        case RGBColor::NormalizeMode::Scale: return normalizeScale(*this);
    }
    assert(false);
    return ERR_COLOR;
}

HSVColor::HSVColor(const RGBColor& rgb) : h(0.0f), a(rgb.a) {
    v = std::max({rgb.r, rgb.g, rgb.b});
    const float C = v - std::min({rgb.r, rgb.g, rgb.b});
    s = (v == 0) ? 0 : C / v;
    if (C > 0) {
        if (v == rgb.r) {
            h = 60 * (rgb.g - rgb.b)/C;
        } else if (v == rgb.g) {
            h = (60 * (rgb.b - rgb.r)/C) + 120;
        } else {
            h = (60 * (rgb.r - rgb.g)/C) + 240;
        }

        if (h < 0) {
            h += 360;
        }
    }
}

RGBColor::RGBColor(const HSVColor& hsv) : a(hsv.a) {
    const float chroma = hsv.s * hsv.v;
    auto X = static_cast<float>(chroma * (1- std::abs(std::fmod(hsv.h/60, 2) - 1)));
    const float m = hsv.v - chroma;
    if (hsv.h < 60)
        r = chroma, g = X, b = 0;
    else if (hsv.h < 120)
        r = X, g = chroma, b = 0;
    else if (hsv.h < 180)
        r = 0, g = chroma, b = X;
    else if (hsv.h < 240)
        r = 0, g = X, b = chroma;
    else if (hsv.h < 300)
        r = X, g = 0, b = chroma;
    else
        r = chroma, g = 0, b = X;

    r += m;
    g += m;
    b += m;
}

} // namepsace Airship
