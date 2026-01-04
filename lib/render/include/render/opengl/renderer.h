#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
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

template <typename VertexT>
struct Mesh {
    using vao_id = unsigned int;
    using buffer_id = unsigned int;
    Mesh(const std::vector<VertexT>& vertices);
    Mesh();
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

private:
    [[nodiscard]] vao_id createVertexArrayObject() const;
    void bindVertexArrayObject() const;

    [[nodiscard]] buffer_id createBuffer() const;
    void bindBuffer() const;
    void copyBuffer(size_t bytes, const void* data) const;

    vao_id m_VertexArrayObject;
    buffer_id m_BufferArrayObject;
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

class Renderer {
public:
    Renderer() = default;
    using program_id = unsigned int;
    void init();
    void resize(int width, int height) const;

    [[nodiscard]] program_id createPipeline(const Shader& vShader, const Shader& fShader) const;
    [[nodiscard]] std::string getLinkLog(program_id pid) const;
    void bindProgram(program_id pid) const;

    template <typename VertexT>
    void draw(std::vector<Mesh<VertexT>>& meshes, bool clear = true) const;
    template <typename VertexT>
    void draw(Mesh<VertexT>& meshes, bool clear = true) const;
    void setClearColor(const RGBColor& color);

private:
    Color m_ClearColor = Colors::Magenta;
};
} // namespace Airship
