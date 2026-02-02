#pragma once

// File to keep track of potential addons to the engine -- things
// we like or use enough to warrant merging in at some point

#include <cassert>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opengl/renderer.h"

inline float randomRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    auto ret = dis(gen);
    return ret;
}

// Each mesh owns a vector of streams. Each stream is associated with:
// - One vertex attribute
// - One buffer (potentially used in multiple streams)
// - One vector of CPU-side data
// This class: owns the CPU data with invalidation logic, and buffer (and defines
// the stream that gets copied into the mesh).
// (i.e. restricts each buffer to hold one vertex attribute)
// The stream is then retrievable from this struct.
class IOwningStream {
public:
    virtual ~IOwningStream() = default;
    virtual void sync() = 0;
    [[nodiscard]] virtual Airship::VertexAttributeStream getStream() const = 0;

    void invalidate() { m_Invalid = true; }

protected:
    bool m_Invalid = false;
};

template <typename T>
class OwningStream : public IOwningStream {
public:
    OwningStream(Airship::ShaderDataType format) : m_Format(format) {};
    virtual ~OwningStream() = default;
    std::vector<T>& data() { return m_Data; }
    void sync() override {
        if (m_Invalid) {
            m_Buffer.update(sizeof(T) * m_Data.size(), m_Data.data());
            m_Invalid = false;
        }
    }
    [[nodiscard]] Airship::VertexAttributeStream getStream() const override {
        return {.buffer = &m_Buffer, .stride = sizeof(T), .offset = 0, .format = m_Format};
    }

protected:
    std::vector<T> m_Data;
    Airship::Buffer m_Buffer;
    Airship::ShaderDataType m_Format;
};

// The dynamic mesh owns a mapping of names to OwningStreams.
// The APIs are templated to keep strong typing when interacting with
// this object, but internally (and in the streams) the data is type-erased.
// By owning the streams, we can keep all of the data used for a mesh owned by one
// object, and force synchronization when drawing (assuming stream::invalidate() is
// used appropriately).
class DynamicMesh : public Airship::Mesh {
public:
    DynamicMesh() = default;
    DynamicMesh(const DynamicMesh&) = delete;
    DynamicMesh operator=(const DynamicMesh&) = delete;
    DynamicMesh(DynamicMesh&&) = default;
    DynamicMesh& operator=(DynamicMesh&&) = default;
    template <typename T>
    OwningStream<T>& addStream(const std::string& name, Airship::ShaderDataType format) {
        auto stream = std::make_unique<OwningStream<T>>(format);
        setAttributeStream(name, stream->getStream());
        m_Streams[name] = std::move(stream);
        return *dynamic_cast<OwningStream<T>*>(m_Streams.at(name).get());
    }

    template <typename T>
    std::vector<T>& getData(const std::string& name) const {
        return getStream<T>(name).data();
    }

    template <typename T>
    OwningStream<T>& getStream(const std::string& name) const {
        assert(m_Streams.contains(name));
        auto ptr = dynamic_cast<OwningStream<T>*>(m_Streams.at(name).get());
        assert(ptr != nullptr);
        return *ptr;
    }

    void draw(const Airship::Renderer& renderer, Airship::Pipeline* pipeline) {
        for (auto& it : m_Streams) {
            it.second->sync();
        }
        renderer.draw(*this, *pipeline, false);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IOwningStream>> m_Streams;
};
