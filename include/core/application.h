#pragma once

#include "core/window.h"

#include <memory>
#include <optional>
#include <vector>

namespace Airship {
    class Application
    {
    public:
        void Run();

    protected:
        std::optional<Window*> CreateWindow(int w, int h);

        virtual void OnStart() {}

    private:
        std::vector<std::unique_ptr<Window>> m_Windows;
    };
} // namespace Airship
