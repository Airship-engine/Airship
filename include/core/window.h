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
            GLFW_CHECK(window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));

            // Should we allow background window creation? Not sure if that's a thing...
            // OpenGL-specific, too
            GLFW_CHECK(glfwMakeContextCurrent(window));

            // Handle resize callback - store this pointer to get the appropriate function pointer.
            GLFW_CHECK(glfwSetWindowUserPointer(window, this));
            GLFW_CHECK(glfwSetFramebufferSizeCallback(window, windowResizeCallback));
        }

        ~Window() {
            if (window) GLFW_CHECK(glfwDestroyWindow(window));
        }

        Utils::Point<int, 2> GetSize() const {
            int width, height;
            GLFW_CHECK(glfwGetWindowSize(window, &width, &height));

            return Utils::Point<int, 2>(width, height);
        }

        GLFWwindow * Get() { return window; }
        void setWindowResizeCallback(resize_callback fn) {
            resizeCallback = fn;
        };

        void swapBuffers() const {
            glfwSwapBuffers(window);
        }

        bool shouldClose() const {
            return glfwWindowShouldClose(window) != 0;
        }

        void pollEvents() const {
            glfwPollEvents();
        }

    private:
        GLFWwindow* window;
        resize_callback resizeCallback;

        static void windowResizeCallback(GLFWwindow *window, int width, int height) {
            Window *win = nullptr;
            GLFW_CHECK(win = static_cast<Window*>(glfwGetWindowUserPointer(window)));
            if (win && win->resizeCallback) {
                win->resizeCallback(width, height);
            }
        }
    };
} // namespace Airship
