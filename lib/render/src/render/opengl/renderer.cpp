#define NOGDI

#include "render/opengl/renderer.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
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

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_GL_ERROR()                                                                                               \
    {                                                                                                                  \
        GLenum err;                                                                                                    \
        while ((err = glGetError()) != GL_NO_ERROR) {                                                                  \
            SHIPLOG_ERROR("OpenGL error: {:X}", err);                                                                  \
            std::abort();                                                                                              \
        }                                                                                                              \
    }

struct VertexFormatInfo {
    int components;
    GLenum type;
    GLboolean normalized;
};

namespace {

VertexFormatInfo getVertexFormatInfo(ShaderDataType format) {
    switch (format) {
    case ShaderDataType::Float:
        return {.components = 1, .type = GL_FLOAT, .normalized = GL_FALSE};
    case ShaderDataType::Float2:
        return {.components = 2, .type = GL_FLOAT, .normalized = GL_FALSE};
    case ShaderDataType::Float3:
        return {.components = 3, .type = GL_FLOAT, .normalized = GL_FALSE};
    case ShaderDataType::Float4:
        return {.components = 4, .type = GL_FLOAT, .normalized = GL_FALSE};
    }
    SHIPLOG_ERROR("Unable to get vertex format info");
    return {};
}

VertexArray setupVertexArrayBinding(const Mesh& mesh, const Pipeline& pipeline) {
    VertexArray vao;
    uint32_t nextBinding = 0;
    SHIPLOG_DEBUG("Setting up vertex input bindings - {} pipeline attributes", pipeline.getVertexAttributes().size());

    for (const auto& attr : pipeline.getVertexAttributes()) {
        const VertexAttributeStream* stream = mesh.getStream(attr.name);
        assert(stream && "Shader requires missing vertex attribute");
        assert(stream->format == attr.format);

        uint32_t binding = nextBinding++;
        SHIPLOG_DEBUG("Binding attribute '{}' to binding {}", attr.name, binding);
        SHIPLOG_DEBUG(" - buffer ID: {}", stream->buffer->get());
        SHIPLOG_DEBUG(" - stride: {}", stream->stride);
        SHIPLOG_DEBUG(" - offset: {}", stream->offset);

        glVertexArrayVertexBuffer(vao.id(), binding, stream->buffer->get(), stream->offset,
                                  static_cast<GLsizei>(stream->stride));
        CHECK_GL_ERROR();

        glEnableVertexArrayAttrib(vao.id(), attr.location);
        CHECK_GL_ERROR();

        auto info = getVertexFormatInfo(attr.format);
        SHIPLOG_DEBUG(" - location: {}", attr.location);
        SHIPLOG_DEBUG(" - components: {}", info.components);
        SHIPLOG_DEBUG(" - type: {}", info.type);
        SHIPLOG_DEBUG(" - normalized: {}", info.normalized);
        glVertexArrayAttribFormat(vao.id(), attr.location, info.components, info.type, info.normalized, 0);
        CHECK_GL_ERROR();

        glVertexArrayAttribBinding(vao.id(), attr.location, binding);
        CHECK_GL_ERROR();
        GLint size = 0;
        glGetNamedBufferParameteriv(stream->buffer->get(), GL_BUFFER_SIZE, &size);
        CHECK_GL_ERROR();
        SHIPLOG_DEBUG("Buffer {} has size: {}", stream->buffer->get(), size);
    }
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
} // anonymous namespace

void Mesh::draw() const {
    assert(m_VertexCount % 3 == 0);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    CHECK_GL_ERROR();
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
Buffer::Buffer() {
    // TODO: allow batch creation of buffers
    glCreateBuffers(1, &m_BufferID);
    SHIPLOG_TRACE("Created buffer with ID {}", m_BufferID);
    CHECK_GL_ERROR();
}

Buffer::Buffer(Buffer&& other) noexcept : m_BufferID(other.m_BufferID) {
    other.m_BufferID = GL_INVALID_VALUE;
}

Buffer::~Buffer() {
    SHIPLOG_TRACE("Deleting buffer with ID {}", m_BufferID);
    glDeleteBuffers(1, &m_BufferID);
    CHECK_GL_ERROR();
}

void Buffer::bind() const {
    SHIPLOG_TRACE("Binding buffer with ID {}", m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    CHECK_GL_ERROR();
}

void Buffer::update(size_t bytes, const void* data) const {
    // GL_STATIC_DRAW: Set data once, used many times.
    // TODO: Implement switching to GL_STREAM_DRAW or GL_DYNAMIC_DRAW
    SHIPLOG_TRACE("Updating buffer {} with {} bytes of data", m_BufferID, bytes);
    if (!glIsBuffer(m_BufferID)) {
        SHIPLOG_ERROR("Attempting to update invalid buffer {}", m_BufferID);
    };
    glNamedBufferData(m_BufferID, static_cast<GLsizeiptr>(bytes), data, GL_STATIC_DRAW);
    CHECK_GL_ERROR();
}

VertexArray::VertexArray() {
    // TODO: Allow batch creation of VAOs
    glCreateVertexArrays(1, &m_VertexArrayID);
    SHIPLOG_TRACE("Created vertex array with ID {}", m_VertexArrayID);
    CHECK_GL_ERROR();
}

VertexArray::~VertexArray() {
    SHIPLOG_TRACE("Deleting vertex array with ID {}", m_VertexArrayID);
    glDeleteVertexArrays(1, &m_VertexArrayID);
    CHECK_GL_ERROR();
}

void VertexArray::bind() const {
    glBindVertexArray(m_VertexArrayID);
    CHECK_GL_ERROR();
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_VertexArrayID(other.m_VertexArrayID) {
    other.m_VertexArrayID = GL_INVALID_VALUE;
}

void Renderer::init() {
    if (gl3wInit() != 0) {
        SHIPLOG_MAYDAY("Unable to initialize gl3w");
        std::abort();
    }
    glEnable(GL_BLEND);
    CHECK_GL_ERROR();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR();
}

void Renderer::resize(int width, int height) const {
    SHIPLOG_TRACE("Window resized to {}x{}", width, height);
    glViewport(0, 0, width, height);
    CHECK_GL_ERROR();
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
    CHECK_GL_ERROR();
}

Shader Shader::from_file(ShaderType type, const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        SHIPLOG_ERROR("Cannot open shader file: {}", filename);
        assert(false);
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return Shader(type, buffer.str());
}

std::string Shader::getCompileLog() const {
    int len;
    glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.resize(len);
    glGetShaderInfoLog(m_ShaderID, len, nullptr, ret.data());
    CHECK_GL_ERROR();
    return ret;
}

Shader::~Shader() {
    SHIPLOG_TRACE("Deleting shader with ID {}", m_ShaderID);
    glDeleteShader(m_ShaderID);
    CHECK_GL_ERROR();
}

Pipeline::Pipeline(const Shader& vShader, const Shader& fShader, const std::vector<VertexAttributeDesc>& attribs) :
    m_ProgramID(glCreateProgram()), m_VertexAttribs(attribs) {
    SHIPLOG_TRACE("Linking pipeline {}", m_ProgramID);
    for (const auto& attr : attribs) {
        (void) attr; // Possibly unused after stripping
        SHIPLOG_TRACE(" - attribute '{}' at location {}", attr.name, attr.location);
    }
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
    CHECK_GL_ERROR();
}

std::string Pipeline::getLinkLog() const {
    int len;
    glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &len);
    std::string ret;
    ret.resize(len);
    glGetProgramInfoLog(m_ProgramID, len, nullptr, ret.data());
    CHECK_GL_ERROR();
    return ret;
}

void Pipeline::bind() const {
    assert(m_ProgramID != 0);
    glUseProgram(m_ProgramID);
    CHECK_GL_ERROR();
}

Pipeline::~Pipeline() {
    SHIPLOG_TRACE("Deleting pipeline {}", m_ProgramID);
    glDeleteProgram(m_ProgramID);
    CHECK_GL_ERROR();
    m_ProgramID = 0;
}

void Renderer::draw(std::vector<Mesh>& meshes, const Pipeline& pipeline, bool clear) const {
    if (clear) {
        glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);
        CHECK_GL_ERROR();
    }
    for (auto& mesh : meshes)
        draw(mesh, pipeline, false);
}

void Renderer::draw(Mesh& mesh, const Pipeline& pipeline, bool clear) const {
    SHIPLOG_TRACE("Drawing mesh with {} vertices", mesh.vertexCount());
    if (clear) {
        glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);
        CHECK_GL_ERROR();
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

} // namespace Airship
