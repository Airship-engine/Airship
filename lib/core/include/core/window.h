#pragma once

#include "logging.h"
#include "utils.hpp"

#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>
#include <optional>
#include <string>

#define GLFW_CHECK(x) \
    do { \
        x; \
        const char *description; \
        [[maybe_unused]] \
        int code = glfwGetError(&description); \
        if (description) \
            SHIPLOG_MAYDAY("glfw error {} -> {}", code, description); \
    } while (false)

namespace Airship {
    class Window
    {
        using resize_callback = std::function<void(int, int)>;
    public:
        Window(int w, int h, const std::string &title, bool visible = true)
        {
            // TODO: Change these hints when using Vulkan. These are set up for opengl specifically.
            GLFW_CHECK(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
            GLFW_CHECK(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
            GLFW_CHECK(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));

            GLFW_CHECK(glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE));
            GLFW_CHECK(m_Window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));

            // Should we allow background window creation? Not sure if that's a thing...
            // OpenGL-specific, too
            GLFW_CHECK(glfwMakeContextCurrent(m_Window));

            // Handle resize callback - store this pointer to get the appropriate function pointer.
            GLFW_CHECK(glfwSetWindowUserPointer(m_Window, this));
            GLFW_CHECK(glfwSetFramebufferSizeCallback(m_Window, windowResizeCallback));
        }

        ~Window() {
            if (m_Window) GLFW_CHECK(glfwDestroyWindow(m_Window));
        }

        Utils::Point<int, 2> GetSize() const {
            int width, height;
            GLFW_CHECK(glfwGetWindowSize(m_Window, &width, &height));

            return Utils::Point<int, 2>(width, height);
        }

        GLFWwindow * Get() { return m_Window; }
        void setWindowResizeCallback(resize_callback fn) {
            m_ResizeCallback = fn;
        };

        void swapBuffers() const {
            glfwSwapBuffers(m_Window);
        }

        bool shouldClose() const {
            return glfwWindowShouldClose(m_Window) != 0;
        }

        void pollEvents() const {
            glfwPollEvents();
        }

    private:
        GLFWwindow* m_Window;
        resize_callback m_ResizeCallback;

        static void windowResizeCallback(GLFWwindow *window, int width, int height) {
            Window *win = nullptr;
            GLFW_CHECK(win = static_cast<Window*>(glfwGetWindowUserPointer(window)));
            if (win && win->m_ResizeCallback) {
                win->m_ResizeCallback(width, height);
            }
        }
    };
} // namespace Airship
