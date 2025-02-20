
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

void Renderer::init() {
    if (gl3wInit()) {
        SHIPLOG_MAYDAY("Unable to initialize gl3w");
        std::abort();
    }
}

void Renderer::resize(int width, int height) {
    glViewport(0, 0, width, height);
}

vao_id Renderer::createVertexArrayObject() {
    // TODO: Allow batch creation of VAOs
    vao_id vid;
    glCreateVertexArrays(1, &vid);
    return vid;
}

void Renderer::bindVertexArrayObject(vao_id vid) {
    glBindVertexArray(vid);
}

buffer_id Renderer::createBuffer() {
    // TODO: allow batch creation of buffers
    buffer_id ret;
    glGenBuffers(1, &ret);
    return ret;
}

void Renderer::bindBuffer(buffer_id buf_id) {
    glBindBuffer(GL_ARRAY_BUFFER, buf_id);
}

void Renderer::copyBuffer(size_t bytes, void *data) {
    // GL_STATIC_DRAW: Set data once, used many times.
    // TODO: Implement switching to GL_STREAM_DRAW or GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW);
}

void Renderer::setVertexAttribDataFloat(int idx, int count) {
    // TODO: Improve flexibility and coverage
    glVertexAttribPointer(idx, count, GL_FLOAT, GL_FALSE, count * sizeof(float), (void *)0);
}

void Renderer::enableVertexAttribArray(int idx) {
    glEnableVertexAttribArray(idx);
}

shader_id Renderer::createShader(ShaderType stype) {
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

program_id Renderer::createProgram() {
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

void Renderer::drawTriangles(int off, int count) {
    // TODO: move clearing to somewhere else, to allow drawing more than one batch of triangles.
    glClearColor(0.39f, 0.582f, 0.926f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, off, count);
}
} // namespace Airship
