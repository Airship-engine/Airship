#pragma once

#include <string>

#define CONCAt2(a, b) a##b
#define CONCAT2(a, b) CONCAt2(a, b)
#define PROFILE_SCOPE(name) ::Airship::Profiling::ScopeTimer CONCAT2(profiler_, __COUNTER__)(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)

namespace Airship::Profiling {

struct ScopeTimer {
public:
    ScopeTimer(const char* name_) noexcept;
    ~ScopeTimer() noexcept; // NOLINT(performance-trivially-destructible)

private:
    [[maybe_unused]] const char* name;
};

void dump([[maybe_unused]] const std::string& filename);

} // namespace Airship::Profiling
