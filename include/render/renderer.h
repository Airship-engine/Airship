#pragma once

#include <string>

namespace Airship {
    using buffer_id = unsigned int;
    using vao_id = unsigned int;
    using shader_id = unsigned int;
    using program_id = unsigned int;
    
    enum class ShaderType {
        Vertex,
        Fragment
    };
    
    class Renderer {
        public:
        void init();
        void resize(int width, int height);
        
        vao_id createVertexArrayObject();
        void bindVertexArrayObject(vao_id vid);
        
        buffer_id createBuffer();
        void bindBuffer(buffer_id buf_id);
        void copyBuffer(size_t bytes, void *data);
        void setVertexAttribDataFloat(int idx, int count);
        void enableVertexAttribArray(int idx);
        
        shader_id createShader(ShaderType stype);
        bool compileShader(shader_id sid, const char *source);
        std::string getCompileLog(shader_id sid);
        void deleteShader(shader_id sid);
        
        program_id createProgram();
        void attachShader(program_id pid, shader_id sid);
        bool linkProgram(program_id pid);
        std::string getLinkLog(program_id pid);
        void bindProgram(program_id pid);
        
        void drawTriangles(int off, int count);
    };
} // namespace Airship
