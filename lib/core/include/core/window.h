#pragma once

#include <GLFW/glfw3.h>
#include <cassert>
#include <functional>
#include <string>
#include <utility>

#include "input.h"
#include "utils.hpp"

namespace Airship {
class Window {
    using resize_callback = std::function<void(int, int)>;
    using keypress_callback = std::function<void(const Window&, Input::Key, int, Input::KeyAction, Input::KeyMods)>;

public:
    static void Init();
    static void Terminate();
    Window(int w, int h, const std::string& title, bool visible = true);
    ~Window();

    [[nodiscard]] Utils::Point<int, 2> GetSize() const;

    // TODO: Remove this function to make GLFW a private dependency
    [[nodiscard]] GLFWwindow* Get() const { return m_Window; }
    void setWindowResizeCallback(resize_callback fn) { m_ResizeCallback = std::move(fn); };
    void setKeyPressCallback(keypress_callback fn) { m_KeypressCallback = std::move(fn); };

    void swapBuffers() const;

    [[nodiscard]] bool shouldClose() const;

    void pollEvents() const;
    void handleResizeEvent(int width, int height) const {
        if (m_ResizeCallback == nullptr) return;
        m_ResizeCallback(width, height);
    }
    void handleKeyPress(Input::Key key, int scancode, Input::KeyAction action, Input::KeyMods mods) const {
        if (m_KeypressCallback == nullptr) return;
        m_KeypressCallback(*this, key, scancode, action, mods);
    }

private:
    GLFWwindow* m_Window;
    resize_callback m_ResizeCallback;
    keypress_callback m_KeypressCallback;
};
} // namespace Airship
