#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "logging.h"
#include "render/color.h"

namespace Airship {

// RAII buffer wrapper
struct Buffer {
    using buffer_id = unsigned int;
    Buffer();
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) noexcept;
    ~Buffer();
    [[nodiscard]] buffer_id get() const { return m_BufferID; }
    void bind() const;
    void update(size_t bytes, const void* data) const;

private:
    buffer_id m_BufferID;
};

enum class ShaderDataType : uint8_t {
    Float,
    Float2,
    Float3,
    Float4
};

struct VertexAttributeStream {
    const Buffer* buffer;
    uint32_t stride;
    uint32_t offset;
    ShaderDataType format;
};

struct Mesh {
    using vao_id = unsigned int;
    void draw() const;
    [[nodiscard]] const VertexAttributeStream* getStream(const std::string& name) const {
        if (!m_VertexAttributeStreams.contains(name)) {
            SHIPLOG_ALERT("Vertex stream '{}' not found", name);
            return nullptr;
        }
        return &m_VertexAttributeStreams.at(name);
    }
    void setAttributeStream(const std::string& name, const VertexAttributeStream& stream) {
        m_VertexAttributeStreams[name] = stream;
    }
    void setVertexCount(int count) { m_VertexCount = count; }
    [[nodiscard]] int vertexCount() const { return m_VertexCount; }

private:
    int m_VertexCount = 0;
    std::unordered_map<std::string, VertexAttributeStream> m_VertexAttributeStreams;
};

enum class ShaderType : uint8_t {
    Vertex,
    Fragment
};

class Shader {
    using shader_id = unsigned int;

public:
    Shader(ShaderType type, const std::string& source);
    [[nodiscard]] shader_id get() const { return m_ShaderID; }
    ~Shader();

private:
    [[nodiscard]] std::string getCompileLog() const;
    shader_id m_ShaderID;
};

class Pipeline {
    using program_id = unsigned int;

public:
    // Abstraction on per-vertex shader variables, e.g.
    // layout (location = 0) in vec3 aPos
    struct VertexAttributeDesc {
        std::string name;
        uint32_t location;
        ShaderDataType format;
    };

    Pipeline(const Shader& vShader, const Shader& fShader, const std::vector<VertexAttributeDesc>& attribs = {});
    Pipeline(const Pipeline& other) = delete;
    Pipeline(Pipeline&& other) noexcept {
        std::swap(m_ProgramID, other.m_ProgramID);
        std::swap(m_VertexAttribs, other.m_VertexAttribs);
    }
    Pipeline& operator=(const Pipeline& other) = delete;
    Pipeline& operator=(Pipeline&& other) noexcept {
        std::swap(m_ProgramID, other.m_ProgramID);
        return *this;
    }
    ~Pipeline();
    void bind() const;
    [[nodiscard]] const std::vector<VertexAttributeDesc>& getVertexAttributes() const { return m_VertexAttribs; }

private:
    [[nodiscard]] std::string getLinkLog() const;
    program_id m_ProgramID = 0;
    std::vector<VertexAttributeDesc> m_VertexAttribs;
};

class Renderer {
public:
    Renderer() = default;
    void init();
    void resize(int width, int height) const;

    void draw(std::vector<Mesh>& meshes, const Pipeline& pipeline, bool clear = true) const;
    void draw(Mesh& meshes, const Pipeline& pipeline, bool clear = true) const;
    void setClearColor(const RGBColor& color);

private:
    Color m_ClearColor = Colors::Magenta;
};
} // namespace Airship
