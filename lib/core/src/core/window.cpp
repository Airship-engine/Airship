#include "window.h"

#include <cassert>
#include <string>

#include "GLFW/glfw3.h"
#include "input.h"
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

namespace {
Input::Key GLFWKeyToAirship(int key) {
    switch (key) {
        // clang-format off
        case GLFW_KEY_UNKNOWN: return Input::Key::Unknown;
        case GLFW_KEY_A: return Input::Key::A;
        case GLFW_KEY_B: return Input::Key::B;
        case GLFW_KEY_C: return Input::Key::C;
        case GLFW_KEY_D: return Input::Key::D;
        case GLFW_KEY_E: return Input::Key::E;
        case GLFW_KEY_F: return Input::Key::F;
        case GLFW_KEY_G: return Input::Key::G;
        case GLFW_KEY_H: return Input::Key::H;
        case GLFW_KEY_I: return Input::Key::I;
        case GLFW_KEY_J: return Input::Key::J;
        case GLFW_KEY_K: return Input::Key::K;
        case GLFW_KEY_L: return Input::Key::L;
        case GLFW_KEY_M: return Input::Key::M;
        case GLFW_KEY_N: return Input::Key::N;
        case GLFW_KEY_O: return Input::Key::O;
        case GLFW_KEY_P: return Input::Key::P;
        case GLFW_KEY_Q: return Input::Key::Q;
        case GLFW_KEY_R: return Input::Key::R;
        case GLFW_KEY_S: return Input::Key::S;
        case GLFW_KEY_T: return Input::Key::T;
        case GLFW_KEY_U: return Input::Key::U;
        case GLFW_KEY_V: return Input::Key::V;
        case GLFW_KEY_W: return Input::Key::W;
        case GLFW_KEY_X: return Input::Key::X;
        case GLFW_KEY_Y: return Input::Key::Y;

        case GLFW_KEY_0: return Input::Key::Num0;
        case GLFW_KEY_1: return Input::Key::Num1;
        case GLFW_KEY_2: return Input::Key::Num2;
        case GLFW_KEY_3: return Input::Key::Num3;
        case GLFW_KEY_4: return Input::Key::Num4;
        case GLFW_KEY_5: return Input::Key::Num5;
        case GLFW_KEY_6: return Input::Key::Num6;
        case GLFW_KEY_7: return Input::Key::Num7;
        case GLFW_KEY_8: return Input::Key::Num8;
        case GLFW_KEY_9: return Input::Key::Num9;

        case GLFW_KEY_ESCAPE: return Input::Key::Escape;
        case GLFW_KEY_ENTER: return Input::Key::Enter;
        case GLFW_KEY_TAB: return Input::Key::Tab;
        case GLFW_KEY_BACKSPACE: return Input::Key::Backspace;
        case GLFW_KEY_SPACE: return Input::Key::Space;

        case GLFW_KEY_LEFT: return Input::Key::Left;
        case GLFW_KEY_UP: return Input::Key::Up;
        case GLFW_KEY_RIGHT: return Input::Key::Right;
        case GLFW_KEY_DOWN: return Input::Key::Down;

        case GLFW_KEY_LEFT_SHIFT: return Input::Key::LeftShift;
        case GLFW_KEY_RIGHT_SHIFT: return Input::Key::RightShift;
        case GLFW_KEY_LEFT_CONTROL: return Input::Key::LeftCtrl;
        case GLFW_KEY_RIGHT_CONTROL: return Input::Key::RightCtrl;
        case GLFW_KEY_LEFT_ALT: return Input::Key::LeftAlt;
        case GLFW_KEY_RIGHT_ALT: return Input::Key::RightAlt;
        default: assert(!"Invalid key press");
        // clang-format on
    }
    return Input::Key::Unknown;
}

Input::KeyAction GLFWKeyActionToAirship(int action) {
    switch (action) {
        // clang-format off
        case GLFW_PRESS: return Input::KeyAction::Press;
        case GLFW_RELEASE: return Input::KeyAction::Release;
        case GLFW_REPEAT: return Input::KeyAction::Held;
        default: assert(!"Invalid key action");
        // clang-format on
    }
    return Input::KeyAction::Release;
}

Input::KeyMods GLFWKeyModsToAirship(int mods) {
    switch (mods) {
        // clang-format off
        case 0: return Input::KeyMods::None;
        case GLFW_MOD_SHIFT: return Input::KeyMods::Shift;
        case GLFW_MOD_CONTROL: return Input::KeyMods::Ctrl;
        case GLFW_MOD_ALT: return Input::KeyMods::Alt;
        case GLFW_MOD_SUPER: return Input::KeyMods::Super;
        default: assert(!"Invalid key mods");
        // clang-format on
    }
    return Input::KeyMods::None;
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    GLFW_CHECK();
    if (win == nullptr) return;
    win->handleResizeEvent(width, height);
}

void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win == nullptr) return;
    Input::Key airshipKey = GLFWKeyToAirship(key);
    Input::KeyAction airshipAction = GLFWKeyActionToAirship(action);
    Input::KeyMods airshipMods = GLFWKeyModsToAirship(mods);
    win->handleKeyPress(airshipKey, scancode, airshipAction, airshipMods);
}
} // namespace

void Window::Init() {
    glfwInit();
    GLFW_CHECK();
}

void Window::Terminate() {
    glfwTerminate();
    GLFW_CHECK();
}

Window::Window(int w, int h, const std::string& title, bool visible) {
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
    glfwSetKeyCallback(m_Window, keyPressCallback);
    GLFW_CHECK();
}

Window::~Window() {
    if (m_Window != nullptr) {
        glfwDestroyWindow(m_Window);
        GLFW_CHECK();
    }
}

Utils::Point<int, 2> Window::GetSize() const {
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    GLFW_CHECK();

    return Utils::Point<int, 2>(width, height);
}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_Window);
    GLFW_CHECK();
}

bool Window::shouldClose() const {
    int ret = glfwWindowShouldClose(m_Window);
    GLFW_CHECK();
    return ret != 0;
}
void Window::pollEvents() const {
    glfwPollEvents();
    GLFW_CHECK();
}
} // namespace Airship