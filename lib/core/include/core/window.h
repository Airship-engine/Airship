#pragma once

#include <GLFW/glfw3.h>
#include <cassert>
#include <functional>
#include <string>
#include <utility>

#include "utils.hpp"

namespace Airship {
class Window {
    using resize_callback = std::function<void(int, int)>;

public:
    static void Init();
    static void Terminate();
    Window(int w, int h, const std::string& title, bool visible = true);
    ~Window();

    [[nodiscard]] Utils::Point<int, 2> GetSize() const;

    // TODO: Remove this function to make GLFW a private dependency
    [[nodiscard]] GLFWwindow* Get() const { return m_Window; }
    void setWindowResizeCallback(resize_callback fn) { m_ResizeCallback = std::move(fn); };

    void swapBuffers() const;

    [[nodiscard]] bool shouldClose() const;

    void pollEvents() const;
    void handleResizeEvent(int width, int height) const {
        if (m_ResizeCallback == nullptr) return;
        m_ResizeCallback(width, height);
    }

private:
    GLFWwindow* m_Window;
    resize_callback m_ResizeCallback;
};
} // namespace Airship
