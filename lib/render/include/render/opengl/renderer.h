#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "core/utils.hpp"
#include "render/color.h"

namespace Airship {

struct VertexP {
    VertexP() = default;
    VertexP(const Utils::Point<float, 3>& pos) : m_Position(pos) {}
    static void setAttribData();

    Utils::Point<float, 3> m_Position;
};

struct VertexPC {
    VertexPC() = default;
    VertexPC(const Utils::Point<float, 3>& pos, Color color) : m_Position(pos), m_Color(color) {}
    static void setAttribData();

    Utils::Point<float, 3> m_Position;
    Color m_Color;
};

// RAII buffer wrapper
struct Buffer {
    using buffer_id = unsigned int;
    Buffer();
    ~Buffer();
    [[nodiscard]] buffer_id get() const { return m_BufferID; }
    void bind() const;
    void update(size_t bytes, const void* data) const;

private:
    buffer_id m_BufferID;
};

template <typename VertexT>
struct Mesh {
    using vao_id = unsigned int;
    Mesh() : Mesh(std::vector<VertexT>()) {};
    Mesh(const std::vector<VertexT>& vertices) : m_Vertices(vertices) {};
    Mesh(const Mesh& other) = delete;
    Mesh(Mesh&& other) noexcept {
        std::swap(m_VertexBuffer, other.m_VertexBuffer);
        std::swap(m_Vertices, other.m_Vertices);
        std::swap(m_Invalid, other.m_Invalid);
    }
    Mesh& operator=(const Mesh& other) = delete;
    Mesh& operator=(Mesh&& other) noexcept {
        std::swap(m_VertexBuffer, other.m_VertexBuffer);
        std::swap(m_Vertices, other.m_Vertices);
        std::swap(m_Invalid, other.m_Invalid);
        return *this;
    }
    VertexT& addVertex() {
        m_Invalid = true;
        return m_Vertices.emplace_back();
    }
    std::tuple<VertexT&, VertexT&, VertexT&> addTriangle() {
        m_Invalid = true;
        m_Vertices.reserve(m_Vertices.size() + 3);
        auto& v1 = m_Vertices.emplace_back();
        auto& v2 = m_Vertices.emplace_back();
        auto& v3 = m_Vertices.emplace_back();
        return {v1, v2, v3};
    }
    void draw();
    void invalidate() { m_Invalid = true; }
    std::vector<VertexT>& getVertices() { return m_Vertices; }
    [[nodiscard]] const Buffer& buffer() const { return m_VertexBuffer; }

private:
    Buffer m_VertexBuffer;
    std::vector<VertexT> m_Vertices;
    bool m_Invalid = true;
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

enum class VertexFormat : uint8_t {
    Float3,
    Float4
};

class Pipeline {
    using program_id = unsigned int;

public:
    // Abstraction on per-vertex shader variables, e.g.
    // layout (location = 0) in vec3 aPos
    struct VertexAttributeDesc {
        std::string name;
        uint32_t location;
        VertexFormat format;
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

    template <typename VertexT>
    void draw(std::vector<Mesh<VertexT>>& meshes, const Pipeline& pipeline, bool clear = true) const;
    template <typename VertexT>
    void draw(Mesh<VertexT>& meshes, const Pipeline& pipeline, bool clear = true) const;
    void setClearColor(const RGBColor& color);

private:
    Color m_ClearColor = Colors::Magenta;
};
} // namespace Airship
