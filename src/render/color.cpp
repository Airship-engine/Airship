
#include "render/color.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace Airship {

const Color Color::White(1.0f, 1.0f, 1.0f);
const Color Color::Black(0.0f, 0.0f, 0.0f);

const Color Color::LightGrey(0.75f, 0.75f, 0.75f);
const Color Color::Grey(0.5f, 0.5f, 0.5f);
const Color Color::DarkGrey(0.25f, 0.25f, 0.25f);

const Color Color::Red(1.0f, 0.0f, 0.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f);

const Color Color::Cyan(0.0f, 1.0f, 1.0f);
const Color Color::Magenta(1.0f, 0.0f, 1.0f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f);

const Color Color::CornflowerBlue(0.39f, 0.582f, 0.926f);
const Color Color::Orange(1.0f, 0.5f, 0.0f);

static Color blendAlpha(const Color &bg, const Color &fg) {
    float newA = fg.a + bg.a * (1 - fg.a);
    if (newA < SMALL_ALPHA) return Color(0.0f, 0.0f, 0.0f, 0.0f);
    float bgScale = bg.a * (1 - fg.a) / newA;
    float fgScale = fg.a / newA;
    return Color(
        fg.r * fgScale + bg.r * bgScale,
        fg.g * fgScale + bg.g * bgScale,
        fg.b * fgScale + bg.b * bgScale,
        newA
    );
}

static Color blendMultiply(const Color &bg, const Color &fg) {
    float newR = bg.r * fg.r;
    float newG = bg.g * fg.g;
    float newB = bg.b * fg.b;
    float newA = bg.a * fg.a;
    return Color(newR, newG, newB, newA);
}

static Color blendAdd(const Color &bg, const Color &fg) {
    float newR = bg.r + fg.r;
    float newG = bg.g + fg.g;
    float newB = bg.b + fg.b;
    float newA = std::max(bg.a, fg.a);
    return Color(newR, newG, newB, newA);
}

static Color blendAverage(const Color &bg, const Color &fg) {
    float newR = (bg.r + fg.r) / 2;
    float newG = (bg.g + fg.g) / 2;
    float newB = (bg.b + fg.b) / 2;
    float newA = std::max(bg.a, fg.a);
    return Color(newR, newG, newB, newA);
}

Color Color::blend(const Color &bg, const Color &fg, Color::BlendMode mode) {
    switch (mode) {
        case Color::BlendMode::Alpha: return blendAlpha(bg, fg);
        case Color::BlendMode::Multiply: return blendMultiply(bg, fg);
        case Color::BlendMode::Add: return blendAdd(bg, fg);
        case Color::BlendMode::Average: return blendAverage(bg, fg);
    }
    assert(false);
    return Color(0.0f,0.0f,0.0f,0.0f);
}

Color Color::lerp(const Color &bg, const Color &fg, float t) {
    Color A(fg.r * t, fg.g * t, fg.b * t, fg.a * t);
    Color B(bg.r * (1 - t), bg.g * (1 - t), bg.b * (1 - t), bg.a * (1 - t));
    return blend(A, B, Color::BlendMode::Add);
}

static Color compositeOver(const Color &bg, const Color &fg) {
    float newA = fg.a + bg.a * (1 - fg.a);
    if (newA < SMALL_ALPHA) return Color(0.0f, 0.0f, 0.0f, 0.0f);
    float bgScale = bg.a * (1 - fg.a) / newA;
    float fgScale = fg.a / newA;
    return Color(
        fg.r * fgScale + bg.r * bgScale,
        fg.g * fgScale + bg.g * bgScale,
        fg.b * fgScale + bg.b * bgScale,
        newA
    );
}

Color Color::composite(const Color &bg, const Color &fg, Color::CompositeMode mode) {
    switch (mode) {
        case Color::CompositeMode::Over: return compositeOver(bg, fg);
    }
    assert(false);
    return Color(0.0f,0.0f,0.0f,0.0f);
}

static Color normalizeClip(const Color &color) {
    float newR = std::clamp(color.r, 0.0f, 1.0f);
    float newG = std::clamp(color.g, 0.0f, 1.0f);
    float newB = std::clamp(color.b, 0.0f, 1.0f);
    float newA = std::clamp(color.a, 0.0f, 1.0f);
    return Color(newR, newG, newB, newA);
}

static Color normalizeScale(const Color &color) {
    float div = std::max(color.r, color.g);
    div = std::max(div, color.b);
    float newR = color.r / div;
    float newG = color.g / div;
    float newB = color.b / div;
    float newA = std::clamp(color.r, 0.0f, 1.0f);
    return Color(newR, newG, newB, newA);
}

Color Color::normalize(Color::NormalizeMode mode) {
    switch (mode) {
        case Color::NormalizeMode::Clip: return normalizeClip(*this);
        case Color::NormalizeMode::Scale: return normalizeScale(*this);
    }
    assert(false);
    return Color(0.0f,0.0f,0.0f,0.0f);
}

float Chroma(const Color &color) {
    float V = color.V();
    float min = std::min(color.r, color.g);
    min = std::min(min, color.b);
    return V - min;
}

float Color::H() const {
    const float C = Chroma(*this);
    if (C == 0) return 0;
    float ret = 0.0f;
    if (V() == r) {
        ret = 60 * (g - b)/C;
    } else if (V() == g) {
        ret = 60 * (b - r)/C + 120;
    } else {
        ret = 60 * (r - g)/C + 240;
    }
    if (ret < 0) ret += 360;
    return ret;
}

float Color::S() const {
    float C = Chroma(*this);
    if (V() == 0) return 0;
    return C / V();
}

float Color::V() const {
    float V = std::max(r, g);
    return std::max(V, b);
}

Color Color::HSV(float h, float s, float v, float a) {
    float chroma = s * v;
    float X = chroma * (1- std::abs(std::fmod(h/60, 2) - 1));
    float m = v - chroma;
    Color ret = Color::Black;
    if (h < 60)
        ret.r = chroma, ret.g = X, ret.b = 0;
    else if (h < 120)
        ret.r = X, ret.g = chroma, ret.b = 0;
    else if (h < 180)
        ret.r = 0, ret.g = chroma, ret.b = X;
    else if (h < 240)
        ret.r = 0, ret.g = X, ret.b = chroma;
    else if (h < 300)
        ret.r = X, ret.g = 0, ret.b = chroma;
    else
        ret.r = chroma, ret.g = 0, ret.b = X;

    ret.r += m;
    ret.g += m;
    ret.b += m;
    return ret;
}

} // namepsace Airship
