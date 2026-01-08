#pragma once

#include <cstdint>

namespace Airship::Input {

enum class Key : uint8_t {
    Unknown,

    // clang-format off
    A, B, C, D, E,
    F, G, H, I, J,
    K, L, M, N, O,
    P, Q, R, S, T,
    U, V, W, X, Y, Z,

    Num0, Num1, Num2,
    Num3, Num4, Num5,
    Num6, Num7, Num8, Num9,
    // clang-format on

    Escape,
    Enter,
    Tab,
    Backspace,
    Space,

    Left,
    Up,
    Right,
    Down,

    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,
};

enum class KeyAction : uint8_t {
    Press,
    Release,
    Held
};

enum class KeyMods : uint8_t {
    None,
    Shift,
    Ctrl,
    Alt,
    Super,
};

} // namespace Airship::Input