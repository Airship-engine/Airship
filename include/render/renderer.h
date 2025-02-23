#pragma once

#include "core/utils.hpp"

#include <string>
#include <vector>

namespace Airship {

    struct Vertex {
        Vertex(const Utils::Point<float,3> &pos) : m_Position(pos) {}
        static void setAttribData();

        Utils::Point<float, 3> m_Position;
    private:
        static void setVertexAttribDataFloat(int idx, int count);
        static void enableVertexAttribArray(int idx);
    };

    struct Mesh {
        using vao_id = unsigned int;
        using buffer_id = unsigned int;
        Mesh(const std::vector<Vertex> &vertices);
        void draw() const;
    private:
        vao_id createVertexArrayObject() const;
        void bindVertexArrayObject() const;

        buffer_id createBuffer() const;
        void bindBuffer() const;
        void copyBuffer(size_t bytes, const void *data) const;

        vao_id m_VertexArrayObject;
        buffer_id m_BufferArrayObject;
        int m_Count;
    };
    
    enum class ShaderType {
        Vertex,
        Fragment
    };
    
    class Renderer {
    public:
        using shader_id = unsigned int;
        using program_id = unsigned int;
        void init();
        void resize(int width, int height);
        
        shader_id createShader(ShaderType stype);
        bool compileShader(shader_id sid, const char *source);
        std::string getCompileLog(shader_id sid);
        void deleteShader(shader_id sid);
        
        program_id createProgram();
        void attachShader(program_id pid, shader_id sid);
        bool linkProgram(program_id pid);
        std::string getLinkLog(program_id pid);
        void bindProgram(program_id pid);

        void draw(const std::vector<Mesh> &meshes);
    };
} // namespace Airship
