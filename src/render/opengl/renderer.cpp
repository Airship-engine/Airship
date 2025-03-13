
#include "render/renderer.h"
#include "core/logging.h"

#include "GL/gl3w.h"

namespace Airship {

constexpr static GLenum toGL(ShaderType stype) {
    switch (stype) {
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        default: SHIPLOG_ERROR("Unable to convert to GL shader type");
    }
    return 0;
}

void Vertex::setAttribData() {
    // Needs to be expanded if vertex data changes
    setVertexAttribDataFloat(0, 3);
    enableVertexAttribArray(0);
}

void Vertex::setVertexAttribDataFloat(int idx, int count) {
    // TODO: Improve flexibility and coverage
    glVertexAttribPointer(idx, count, GL_FLOAT, GL_FALSE, count * sizeof(float), (void *)0);
}

void Vertex::enableVertexAttribArray(int idx) {
    glEnableVertexAttribArray(idx);
}

Mesh::Mesh(const std::vector<Vertex> &vertices) : m_Count(static_cast<int>(vertices.size())) {
    m_VertexArrayObject = createVertexArrayObject();
    bindVertexArrayObject();

    m_BufferArrayObject = createBuffer();
    bindBuffer();
    copyBuffer(vertices.size()*sizeof(Vertex), vertices.data());

    Vertex::setAttribData();
}

void Mesh::draw() const {
    int off = 0; // Maybe used, maybe always 0?
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
    glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW);
}

void Renderer::init() {
    if (gl3wInit()) {
        SHIPLOG_MAYDAY("Unable to initialize gl3w");
        std::abort();
    }
}

void Renderer::resize(int width, int height) {
    glViewport(0, 0, width, height);
}

Renderer::shader_id Renderer::createShader(ShaderType stype) {
    return glCreateShader(toGL(stype));
}

bool Renderer::compileShader(shader_id sid, const char *source) {
    glShaderSource(sid, 1, &source, NULL);
    glCompileShader(sid);
    int ok;
    glGetShaderiv(sid, GL_COMPILE_STATUS, &ok);
    return ok == GL_TRUE;
}

std::string Renderer::getCompileLog(shader_id sid) {
    int len;
    glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.reserve(len);
    glGetShaderInfoLog(sid, len, nullptr, ret.data());
    return ret;
}

void Renderer::deleteShader(shader_id sid) {
    glDeleteShader(sid);
}

Renderer::program_id Renderer::createProgram() {
    return glCreateProgram();
}

void Renderer::attachShader(program_id pid, shader_id sid) {
    glAttachShader(pid, sid);
}

bool Renderer::linkProgram(program_id pid) {
    glLinkProgram(pid);
    int ok;
    glGetProgramiv(pid, GL_LINK_STATUS, &ok);
    return ok == GL_TRUE;
}

std::string Renderer::getLinkLog(program_id pid) {
    int len;
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.reserve(len);
    glGetProgramInfoLog(pid, len, nullptr, ret.data());
    return ret;
}

void Renderer::bindProgram(program_id pid) {
    glUseProgram(pid);
}

void Renderer::draw(const std::vector<Mesh> &meshes) {
    glClearColor(0.39f, 0.582f, 0.926f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto &mesh : meshes)
        mesh.draw();
}
} // namespace Airship
