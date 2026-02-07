#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "core/logging.h"
#include "render/color.h"

namespace Airship {

// RAII buffer wrapper
struct Buffer {
    using buffer_id = unsigned int;
    Buffer();
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) noexcept;
    ~Buffer();
    [[nodiscard]] buffer_id get() const { return m_BufferID; }
    void bind() const;
    void update(size_t bytes, const void* data);

private:
    buffer_id m_BufferID;
    size_t m_Size = 0;
};

enum class ShaderDataType : uint8_t {
    Float,
    Float2,
    Float3,
    Float4
};

struct VertexAttributeStream {
    const Buffer* buffer;
    uint32_t stride;
    uint32_t offset;
    ShaderDataType format;
};

struct Mesh {
    using vao_id = unsigned int;
    void draw() const;
    [[nodiscard]] const VertexAttributeStream* getStream(const std::string& name) const {
        if (!m_VertexAttributeStreams.contains(name)) {
            SHIPLOG_ALERT("Vertex stream '{}' not found", name);
            return nullptr;
        }
        return &m_VertexAttributeStreams.at(name);
    }
    void setAttributeStream(const std::string& name, const VertexAttributeStream& stream) {
        m_VertexAttributeStreams[name] = stream;
    }
    void setVertexCount(int count) { m_VertexCount = count; }
    [[nodiscard]] int vertexCount() const { return m_VertexCount; }

private:
    int m_VertexCount = 0;
    std::unordered_map<std::string, VertexAttributeStream> m_VertexAttributeStreams;
};

enum class ShaderType : uint8_t {
    Vertex,
    Fragment
};

class Shader {
    using shader_id = unsigned int;

public:
    Shader(ShaderType type, const std::string& source);
    static Shader from_file(ShaderType type, const std::string& filename);
    [[nodiscard]] shader_id get() const { return m_ShaderID; }
    ~Shader();

private:
    [[nodiscard]] std::string getCompileLog() const;
    shader_id m_ShaderID;
};

// Can be extended by the user to set uniforms from user-defined classes
template <typename T>
struct UniformTraits {};

template <>
struct UniformTraits<float> {
    using UniformType = float;
    static UniformType Convert(const float& val) { return val; }
};

template <>
struct UniformTraits<Color> {
    using UniformType = Color;
    static UniformType Convert(const Color& val) { return val; }
};

class Pipeline {
    using program_id = unsigned int;

public:
    // Abstraction on per-vertex shader variables, e.g.
    // layout (location = 0) in vec3 aPos
    struct VertexAttributeDesc {
        std::string name;
        uint32_t location;
        ShaderDataType format;
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
    [[nodiscard]] int GetUniformLocation(const std::string& name) const;

private:
    [[nodiscard]] std::string getLinkLog() const;
    program_id m_ProgramID = 0;
    std::vector<VertexAttributeDesc> m_VertexAttribs;
};

using UniformVariant = std::variant<float, Color>;

template <typename T>
inline ShaderDataType DeduceShaderType() {
    if constexpr (std::is_same_v<T, float>) return ShaderDataType::Float;
    if constexpr (std::is_same_v<T, Color>) return ShaderDataType::Float4;
}

struct UniformValue {
    ShaderDataType type;
    UniformVariant value;
    bool dirty;
};

template <typename T>
concept UniformCompatible = requires(const T& v) {
    typename UniformTraits<T>::UniformType;
    { UniformTraits<T>::Convert(v) };
};

class Material {
public:
    Material(const Pipeline* pipeline) : m_Pipeline(pipeline) {}

    template <UniformCompatible T>
    void SetUniform(const std::string& name, const T& value) {
        using Traits = UniformTraits<T>;
        using UType = typename Traits::UniformType;

        UType converted = Traits::Convert(value);

        auto& u = m_Uniforms[name];
        u.type = DeduceShaderType<UType>();
        u.value = converted;
        u.dirty = true;
    }

    void Bind() const;
    [[nodiscard]] const Pipeline& pipeline() const { return *m_Pipeline; }

private:
    const Pipeline* m_Pipeline;
    std::unordered_map<std::string, UniformValue> m_Uniforms;
};

class Renderer {
public:
    Renderer() = default;
    void init();
    void resize(int width, int height) const;

    void clear() const;
    void draw(const std::vector<Mesh>& meshes, const Material& mat, bool doClear = true) const;
    void draw(const Mesh& mesh, const Material& mat, bool doClear = true) const;
    void setClearColor(const RGBColor& color);

private:
    Color m_ClearColor = Colors::Magenta;
};

} // namespace Airship
