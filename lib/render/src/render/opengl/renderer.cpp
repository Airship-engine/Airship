#define NOGDI

#include "render/opengl/renderer.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "core/logging.h"
#include "render/color.h"

namespace Airship {

// RAII vertex array wrapper
class VertexArray {
public:
    using vao_id = unsigned int;

    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept {
        std::swap(m_VertexArrayID, other.m_VertexArrayID);
        return *this;
    }

    void bind() const;
    [[nodiscard]] vao_id id() const { return m_VertexArrayID; }

private:
    vao_id m_VertexArrayID = GL_INVALID_VALUE;
};

namespace {
template <typename VertexType>
VertexArray setupVertexArrayBinding(const Mesh<VertexType>& mesh, const Pipeline& pipeline) {
    VertexArray vao;
    (void) pipeline; // Unused for now -- will eventually be used as the source of truth for attributes

    vao.bind();
    VertexType::setAttribData();
    glBindVertexBuffer(0, mesh.buffer().get(), 0, sizeof(VertexType));
    return vao;
}

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
void Mesh<VertexT>::draw() {
    auto numVertices = m_Vertices.size();
    assert(numVertices % 3 == 0);
    const int off = 0; // Maybe used, maybe always 0?
    if (m_Invalid) {
        m_VertexBuffer.update(m_Vertices.size() * sizeof(VertexT), m_Vertices.data());
        m_Invalid = false;
    }
    glDrawArrays(GL_TRIANGLES, off, numVertices);
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
Buffer::Buffer() {
    // TODO: allow batch creation of buffers
    glGenBuffers(1, &m_BufferID);
}

void Buffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void Buffer::update(size_t bytes, const void* data) const {
    // GL_STATIC_DRAW: Set data once, used many times.
    // TODO: Implement switching to GL_STREAM_DRAW or GL_DYNAMIC_DRAW
    glNamedBufferData(m_BufferID, static_cast<GLsizeiptr>(bytes), data, GL_STATIC_DRAW);
}

VertexArray::VertexArray() {
    // TODO: Allow batch creation of VAOs
    glCreateVertexArrays(1, &m_VertexArrayID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_VertexArrayID);
}

void VertexArray::bind() const {
    glBindVertexArray(m_VertexArrayID);
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_VertexArrayID(other.m_VertexArrayID) {
    other.m_VertexArrayID = GL_INVALID_VALUE;
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

Shader::Shader(ShaderType stype, const std::string& source) : m_ShaderID(glCreateShader(toGL(stype))) {
    const char* src = source.c_str();
    glShaderSource(m_ShaderID, 1, &src, nullptr);
    glCompileShader(m_ShaderID);
    int ok;
    glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) {
        std::string log = getCompileLog();
        SHIPLOG_ERROR(log);
    }
    assert(ok == GL_TRUE);
}

std::string Shader::getCompileLog() const {
    int len;
    glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.resize(len);
    glGetShaderInfoLog(m_ShaderID, len, nullptr, ret.data());
    return ret;
}

Shader::~Shader() {
    glDeleteShader(m_ShaderID);
}

Pipeline::Pipeline(const Shader& vShader, const Shader& fShader) : m_ProgramID(glCreateProgram()) {
    glAttachShader(m_ProgramID, vShader.get());
    glAttachShader(m_ProgramID, fShader.get());
    glLinkProgram(m_ProgramID);
    int ok;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &ok);
    if (ok != GL_TRUE) {
        std::string log = getLinkLog();
        SHIPLOG_ERROR(log);
    }
    assert(ok == GL_TRUE);
}

std::string Pipeline::getLinkLog() const {
    int len;
    glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.resize(len);
    glGetProgramInfoLog(m_ProgramID, len, nullptr, ret.data());
    return ret;
}

void Pipeline::bind() const {
    assert(m_ProgramID != 0);
    glUseProgram(m_ProgramID);
}

Pipeline::~Pipeline() {
    glDeleteProgram(m_ProgramID);
    m_ProgramID = 0;
}

template <typename VertexT>
void Renderer::draw(std::vector<Mesh<VertexT>>& meshes, const Pipeline& pipeline, bool clear) const {
    if (clear) {
        glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    for (auto& mesh : meshes)
        draw(mesh, pipeline, false);
}

template <typename VertexT>
void Renderer::draw(Mesh<VertexT>& mesh, const Pipeline& pipeline, bool clear) const {
    if (clear) {
        glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    // TODO: Cache VAO per mesh/pipeline combo
    VertexArray vao = setupVertexArrayBinding(mesh, pipeline);
    pipeline.bind();
    vao.bind();
    mesh.draw();
}

void Renderer::setClearColor(const RGBColor& color) {
    m_ClearColor = color;
}

// Explicit instantiations
template struct Mesh<VertexP>;
template struct Mesh<VertexPC>;
template void Renderer::draw<VertexP>(std::vector<Mesh<VertexP>>&, const Pipeline&, bool) const;
template void Renderer::draw<VertexPC>(std::vector<Mesh<VertexPC>>&, const Pipeline&, bool) const;
template void Renderer::draw<VertexP>(Mesh<VertexP>&, const Pipeline&, bool) const;
template void Renderer::draw<VertexPC>(Mesh<VertexPC>&, const Pipeline&, bool) const;

} // namespace Airship
