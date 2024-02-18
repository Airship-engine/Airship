#pragma once

#include <memory>
#include <optional>

namespace Airship {
    class Window
    {
    public:
        Window(int w, int h) : width(w), height(h) {}

    private:
        int width;
        int height;
    };
} // namespace Airship
