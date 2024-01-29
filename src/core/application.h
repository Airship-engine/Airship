#include <memory>
#include <optional>
#include <vector>
#include <cassert>
#include <iostream>

namespace Airship {
    class Window
    {
    public:
        Window(int w, int h) : width(w), height(h) {}

    private:
        int width;
        int height;
    };

    class Application
    {
    public:
        void Run() {
            OnStart();
        }

    protected:
        std::optional<Window*> CreateWindow(int w, int h)
        {
            if (w < 1 || h < 1) return std::nullopt;
            return m_Windows.emplace_back(std::make_unique<Window>(w, h)).get();
        }

        virtual void OnStart() {}

    private:
        std::vector<std::unique_ptr<Window>> m_Windows;
    };
}
