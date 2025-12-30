#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "core/utils.hpp"
#include "render/color.h"

namespace Airship {

struct Vertex {
    Vertex(const Utils::Point<float, 3>& pos) : m_Position(pos) {}
    static void setAttribData();

    Utils::Point<float, 3> m_Position;

private:
    static void setVertexAttribDataFloat(int idx, int count);
    static void enableVertexAttribArray(int idx);
};

struct Mesh {
    using vao_id = unsigned int;
    using buffer_id = unsigned int;
    Mesh(const std::vector<Vertex>& vertices);
    void draw() const;

private:
    [[nodiscard]] vao_id createVertexArrayObject() const;
    void bindVertexArrayObject() const;

    [[nodiscard]] buffer_id createBuffer() const;
    void bindBuffer() const;
    void copyBuffer(size_t bytes, const void* data) const;

    vao_id m_VertexArrayObject;
    buffer_id m_BufferArrayObject;
    int m_Count;
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

    void draw(const std::vector<Mesh>& meshes) const;
    void setClearColor(const RGBColor& color);

private:
    Color m_ClearColor = Colors::Magenta;
};
} // namespace Airship
