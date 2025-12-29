
#include "render/opengl/renderer.h"
#include "GL/glcorearb.h"
#include "core/logging.h"

#include "GL/gl3w.h"
#include "render/color.h"
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

namespace Airship {

namespace {
    constexpr GLenum toGL(ShaderType stype) {
        switch (stype) {
            case ShaderType::Vertex: return GL_VERTEX_SHADER;
            case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
            default: SHIPLOG_ERROR("Unable to convert to GL shader type");
        }
        return 0;
    }
} // anonymous namespace

void Vertex::setAttribData() {
    // Needs to be expanded if vertex data changes
    setVertexAttribDataFloat(0, 3);
    enableVertexAttribArray(0);
}

void Vertex::setVertexAttribDataFloat(int idx, int count) {
    // TODO: Improve flexibility and coverage
    auto stride = static_cast<GLsizei>(count * sizeof(float));
    glVertexAttribPointer(idx, count, GL_FLOAT, GL_FALSE, stride, (void *)nullptr);
}

void Vertex::enableVertexAttribArray(int idx) {
    glEnableVertexAttribArray(idx);
}

Mesh::Mesh(const std::vector<Vertex> &vertices) :
    m_VertexArrayObject(createVertexArrayObject()),
    m_BufferArrayObject(createBuffer()),
    m_Count(static_cast<int>(vertices.size())) {
    
    bindVertexArrayObject();

    bindBuffer();
    copyBuffer(vertices.size()*sizeof(Vertex), vertices.data());

    Vertex::setAttribData();
}

void Mesh::draw() const {
    const int off = 0; // Maybe used, maybe always 0?
    bindVertexArrayObject();
    glDrawArrays(GL_TRIANGLES, off, m_Count);
}

Mesh::vao_id Mesh::createVertexArrayObject() const {
    // TODO: Allow batch creation of VAOs
    vao_id vid;
    glCreateVertexArrays(1, &vid);
    return vid;
}

void Mesh::bindVertexArrayObject() const {
    glBindVertexArray(m_VertexArrayObject);
}

Mesh::buffer_id Mesh::createBuffer() const {
    // TODO: allow batch creation of buffers
    buffer_id ret;
    glGenBuffers(1, &ret);
    return ret;
}

void Mesh::bindBuffer() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferArrayObject);
}

void Mesh::copyBuffer(size_t bytes, const void *data) const {
    // GL_STATIC_DRAW: Set data once, used many times.
    // TODO: Implement switching to GL_STREAM_DRAW or GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes), data, GL_STATIC_DRAW);
}

void Renderer::init() {
    if (gl3wInit() != 0) {
        SHIPLOG_MAYDAY("Unable to initialize gl3w");
        std::abort();
    }
}

void Renderer::resize(int width, int height) const {
    glViewport(0, 0, width, height);
}

Renderer::shader_id Renderer::createShader(ShaderType stype) const {
    return glCreateShader(toGL(stype));
}

bool Renderer::compileShader(shader_id sid, const char *source) const {
    glShaderSource(sid, 1, &source, nullptr);
    glCompileShader(sid);
    int ok;
    glGetShaderiv(sid, GL_COMPILE_STATUS, &ok);
    return ok == GL_TRUE;
}

std::string Renderer::getCompileLog(shader_id sid) const {
    int len;
    glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.reserve(len);
    glGetShaderInfoLog(sid, len, nullptr, ret.data());
    return ret;
}

void Renderer::deleteShader(shader_id sid) const {
    glDeleteShader(sid);
}

Renderer::program_id Renderer::createProgram() const {
    return glCreateProgram();
}

void Renderer::attachShader(program_id pid, shader_id sid) const {
    glAttachShader(pid, sid);
}

bool Renderer::linkProgram(program_id pid) const {
    glLinkProgram(pid);
    int ok;
    glGetProgramiv(pid, GL_LINK_STATUS, &ok);
    return ok == GL_TRUE;
}

std::string Renderer::getLinkLog(program_id pid) const {
    int len;
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.reserve(len);
    glGetProgramInfoLog(pid, len, nullptr, ret.data());
    return ret;
}

void Renderer::bindProgram(program_id pid) const {
    glUseProgram(pid);
}

void Renderer::draw(const std::vector<Mesh> &meshes) const {
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto &mesh : meshes)
        mesh.draw();
}

void Renderer::setClearColor(const RGBColor &color) {
    m_ClearColor = color;
}

} // namespace Airship
