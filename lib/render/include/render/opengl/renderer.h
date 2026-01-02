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

class Renderer {
public:
    Renderer() = default;
    using shader_id = unsigned int;
    using program_id = unsigned int;
    void init();
    void resize(int width, int height) const;

    [[nodiscard]] shader_id createShader(ShaderType stype) const;
    bool compileShader(shader_id sid, const char* source) const;
    [[nodiscard]] std::string getCompileLog(shader_id sid) const;
    void deleteShader(shader_id sid) const;

    [[nodiscard]] program_id createProgram() const;
    void attachShader(program_id pid, shader_id sid) const;
    [[nodiscard]] bool linkProgram(program_id pid) const;
    [[nodiscard]] std::string getLinkLog(program_id pid) const;
    void bindProgram(program_id pid) const;

    template <typename VertexT>
    void draw(std::vector<Mesh<VertexT>>& meshes) const;
    void setClearColor(const RGBColor& color);

private:
    Color m_ClearColor = Colors::Magenta;
};
} // namespace Airship
