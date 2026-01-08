#pragma once

#include <GLFW/glfw3.h>
#include <cassert>
#include <functional>
#include <string>
#include <utility>

#include "logging.h"
#include "utils.hpp"

// NOLINTNEXTLINE
#define GLFW_CHECK()                                                                                                   \
    /* NOLINTBEGIN(cppcoreguidelines-avoid-do-while) */                                                                \
    do {                                                                                                               \
        const char* description;                                                                                       \
        [[maybe_unused]]                                                                                               \
        int code = glfwGetError(&description);                                                                         \
        if (description) SHIPLOG_MAYDAY("glfw error {} -> {}", code, description);                                     \
    } while (0) /* NOLINTEND(cppcoreguidelines-avoid-do-while) */

namespace Airship {
class Window {
    using resize_callback = std::function<void(int, int)>;

public:
    Window(int w, int h, const std::string& title, bool visible = true) {
        // TODO: Change these hints when using Vulkan. These are set up for opengl specifically.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        GLFW_CHECK();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        GLFW_CHECK();
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFW_CHECK();
        glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
        GLFW_CHECK();
        m_Window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
        GLFW_CHECK();

        // Should we allow background window creation? Not sure if that's a thing...
        // OpenGL-specific, too
        glfwMakeContextCurrent(m_Window);
        GLFW_CHECK();

        // Handle resize callback - store this pointer to get the appropriate function pointer.
        glfwSetWindowUserPointer(m_Window, this);
        GLFW_CHECK();
        glfwSetFramebufferSizeCallback(m_Window, windowResizeCallback);
        GLFW_CHECK();
    }

    ~Window() {
        if (m_Window != nullptr) {
            glfwDestroyWindow(m_Window);
            GLFW_CHECK();
        }
    }

    [[nodiscard]] Utils::Point<int, 2> GetSize() const {
        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        GLFW_CHECK();

        return Utils::Point<int, 2>(width, height);
    }

    [[nodiscard]] GLFWwindow* Get() const { return m_Window; }
    void setWindowResizeCallback(resize_callback fn) { m_ResizeCallback = std::move(fn); };

    void swapBuffers() const { glfwSwapBuffers(m_Window); }

    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(m_Window) != 0; }

    void pollEvents() const { glfwPollEvents(); }

private:
    GLFWwindow* m_Window;
    resize_callback m_ResizeCallback;

    static void windowResizeCallback(GLFWwindow* window, int width, int height) {
        const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        GLFW_CHECK();
        if ((win != nullptr) && win->m_ResizeCallback) {
            win->m_ResizeCallback(width, height);
        }
    }
};
} // namespace Airship
