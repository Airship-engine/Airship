#include "core/instrumentation.h"

#ifndef AIRSHIP_INSTRUMENTATION
namespace Airship::Profiling {
ScopeTimer::ScopeTimer(const char* name_) noexcept : name(name_) {}
ScopeTimer::~ScopeTimer() noexcept = default;
void dump([[maybe_unused]] const std::string& filename) {}
} // namespace Airship::Profiling
#else
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace Airship::Profiling {

enum class TraceEventType : uint8_t {
    start,
    end
};

struct TraceEvent {
    const char* name;
    uint64_t timestamp;
    TraceEventType type;
};

struct EventBuffer {
    EventBuffer(uint32_t tid) : threadIndex(tid) {}
    EventBuffer(const EventBuffer&) = delete;
    EventBuffer(const EventBuffer&&) = delete;
    EventBuffer& operator=(const EventBuffer&) = delete;
    EventBuffer& operator=(const EventBuffer&&) = delete;

    uint32_t threadIdx() const { return threadIndex; }
    void emplace_back(const char* name, uint64_t timestamp, TraceEventType type) {
        events.emplace_back(name, timestamp, type);
    }
    TraceEvent get(size_t i) const { return events[i]; }
    size_t count() const { return publishedCount.load(std::memory_order_acquire); }
    void updateCount() { publishedCount = events.size(); }

private:
    // TODO: vectors can reallocate at random - we should refactor to something
    // we have more control over to avoid hiccups
    std::vector<TraceEvent> events;
    uint32_t threadIndex;
    std::atomic<size_t> publishedCount = 0;
};

namespace {
const thread_local uint32_t g_threadIndex = []() noexcept {
    static std::atomic<uint32_t> threadIndex = 0;
    return threadIndex.fetch_add(1, std::memory_order_relaxed);
}();

// Combine thread-local buffers into a globally-accessible variable
std::mutex g_threadBufferMutex;
std::vector<std::unique_ptr<EventBuffer>> g_allEventBuffers;

EventBuffer& GetThreadBuffer() {
    thread_local EventBuffer* buffer = nullptr;
    if (buffer != nullptr) return *buffer;

    // Now we're in normal function context - exceptions are legal
    auto buf = std::make_unique<EventBuffer>(g_threadIndex);

    {
        std::lock_guard<std::mutex> lock(g_threadBufferMutex);
        buffer = g_allEventBuffers.emplace_back(std::move(buf)).get();
    }

    return *buffer;
}

// Lock g_allEventBuffers long enough to copy out the pointers (safe in case we ever spawn+instrument new threads)
// Within each buffer: avoid race conditions by:
// 1. Only pushing into the vectors, never popping
// 2. Periodically publish the size of the vector (to use instead of .size()), indicating which elements are fully
// initialized
std::vector<EventBuffer*> SnapshotEventBuffers() {
    std::lock_guard lock(g_threadBufferMutex);

    std::vector<EventBuffer*> out;
    out.reserve(g_allEventBuffers.size());

    for (const auto& p : g_allEventBuffers)
        out.push_back(p.get());

    return out;
}

void PushEvent(const char* name, TraceEventType type) noexcept {
    // TODO: Add VS profiler integration
    // TODO: Check overhead of std::chrono calls, and potentially use OS implementations
    try {
        auto now = std::chrono::steady_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        EventBuffer& threadBuffer = GetThreadBuffer();
        threadBuffer.emplace_back(name, us, type);

        // Occasionally bump up the published count (every event for now)
        threadBuffer.updateCount();
    } catch (...) {
        // Swallow the error. Worst case scenario, some trace event is never started/stopped. Big whoop.
        (void) 0; // Satisfies bugprone-empty-catch clang-tidy warning
    }
}
} // namespace

ScopeTimer::ScopeTimer(const char* name_) noexcept : name(name_) {
    PushEvent(name, TraceEventType::start);
}

ScopeTimer::~ScopeTimer() noexcept {
    PushEvent(name, TraceEventType::end);
}

// Dump all existing events to a json file, formatted for chrome-tracing/perfetto.ui
void dump(const std::string& filename) {
    auto buffers = SnapshotEventBuffers();

    std::ofstream out(filename);
    out << "{\"traceEvents\":[\n";

    for (auto* buf : buffers) {
        uint32_t end = buf->count();

        for (uint32_t i = 0; i < end; ++i) {
            const TraceEvent& e = buf->get(i);

            out << "{";
            out << R"("name":")" << e.name << "\",";
            out << R"("ph":")" << (e.type == TraceEventType::start ? "B" : "E") << "\",";
            out << "\"ts\":" << e.timestamp << ",";
            out << "\"pid\":0,";
            out << "\"tid\":" << buf->threadIdx();
            out << "},\n";
        }
    }

    out << "{}]}";
}
#endif

} // namespace Airship::Profiling
