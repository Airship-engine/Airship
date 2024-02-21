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
    public:
        Window(int w, int h, const std::string &title, bool visible = true)
        {
            GLFW_CHECK(glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE));
            GLFW_CHECK(window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));
        }

        ~Window() {
            if (window) GLFW_CHECK(glfwDestroyWindow(window));
        }

        Utils::Point<int, 2> GetSize() const {
            int width, height;
            GLFW_CHECK(glfwGetWindowSize(window, &width, &height));

            return Utils::Point<int, 2>(width, height);
        }

    private:
        GLFWwindow* window;
    };
} // namespace Airship
