#define NOGDI

#include "render/opengl/renderer.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "core/logging.h"
#include "render/color.h"

namespace Airship {

namespace {
constexpr GLenum toGL(ShaderType stype) {
    switch (stype) {
    case ShaderType::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderType::Fragment:
        return GL_FRAGMENT_SHADER;
    default:
        SHIPLOG_ERROR("Unable to convert to GL shader type");
    }
    return 0;
}

namespace Vertex {
void setVertexAttribDataFloat(int idx, int count, size_t offset) {
    glVertexAttribFormat(idx, count, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(offset));

    glEnableVertexAttribArray(idx);
    glVertexAttribBinding(idx, 0);
}
} // namespace Vertex
} // anonymous namespace

void VertexP::setAttribData() {
    // Needs to be expanded if vertex data changes
    Vertex::setVertexAttribDataFloat(0, 3, offsetof(VertexP, m_Position));
}

void VertexPC::setAttribData() {
    // Needs to be expanded if vertex data changes
    Vertex::setVertexAttribDataFloat(0, 3, offsetof(VertexPC, m_Position));
    Vertex::setVertexAttribDataFloat(1, 4, offsetof(VertexPC, m_Color));
}

template <typename VertexT>
Mesh<VertexT>::Mesh(const std::vector<VertexT>& vertices) :
    m_VertexArrayObject(createVertexArrayObject()), m_BufferArrayObject(createBuffer()), m_Vertices(vertices) {
    bindVertexArrayObject();
    bindBuffer();

    VertexT::setAttribData();
    glBindVertexBuffer(0, m_BufferArrayObject, 0, sizeof(VertexT));
}

template <typename VertexT>
Mesh<VertexT>::Mesh() : Mesh(std::vector<VertexT>()) {}

template <typename VertexT>
void Mesh<VertexT>::draw() {
    auto numVertices = m_Vertices.size();
    assert(numVertices % 3 == 0);
    const int off = 0; // Maybe used, maybe always 0?
    bindVertexArrayObject();
    if (m_Invalid) {
        bindBuffer();
        copyBuffer(m_Vertices.size() * sizeof(VertexT), m_Vertices.data());
        m_Invalid = false;
    }
    glDrawArrays(GL_TRIANGLES, off, numVertices);
}

template <typename VertexT>
Mesh<VertexT>::vao_id Mesh<VertexT>::createVertexArrayObject() const {
    // TODO: Allow batch creation of VAOs
    vao_id vid;
    glCreateVertexArrays(1, &vid);
    return vid;
}

template <typename VertexT>
void Mesh<VertexT>::bindVertexArrayObject() const {
    glBindVertexArray(m_VertexArrayObject);
}

template <typename VertexT>
Mesh<VertexT>::buffer_id Mesh<VertexT>::createBuffer() const {
    // TODO: allow batch creation of buffers
    buffer_id ret;
    glGenBuffers(1, &ret);
    return ret;
}

template <typename VertexT>
void Mesh<VertexT>::bindBuffer() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferArrayObject);
}

template <typename VertexT>
void Mesh<VertexT>::copyBuffer(size_t bytes, const void* data) const {
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

bool Renderer::compileShader(shader_id sid, const char* source) const {
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
    ret.resize(len);
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
    ret.resize(len);
    glGetProgramInfoLog(pid, len, nullptr, ret.data());
    return ret;
}

void Renderer::bindProgram(program_id pid) const {
    glUseProgram(pid);
}

template <typename VertexT>
void Renderer::draw(std::vector<Mesh<VertexT>>& meshes) const {
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& mesh : meshes)
        mesh.draw();
}

void Renderer::setClearColor(const RGBColor& color) {
    m_ClearColor = color;
}

// Explicit instantiations
template struct Mesh<VertexP>;
template struct Mesh<VertexPC>;
template void Renderer::draw<VertexP>(std::vector<Mesh<VertexP>>&) const;
template void Renderer::draw<VertexPC>(std::vector<Mesh<VertexPC>>&) const;

} // namespace Airship
