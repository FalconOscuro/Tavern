#ifdef USE_OPENGL

#include "tavern/graphics/opengl/shader.h"

#include <GL/glew.h>

#include <boost/log/trivial.hpp>

namespace tavern::graphics::opengl {

shader::shader(const char* vertex_src, const char* fragment_src)
{
    uint32_t v = compile_shader(vertex_src, GL_VERTEX_SHADER);
    uint32_t f = compile_shader(fragment_src, GL_FRAGMENT_SHADER);

    // create program
    m_id = glCreateProgram();
    glAttachShader(m_id, v);
    glAttachShader(m_id, f);
    glLinkProgram(m_id);

    int success = -1;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        const uint32_t LOG_LEN = 1024;
        char log[LOG_LEN];

        glGetProgramInfoLog(m_id, LOG_LEN, NULL, log);

        BOOST_LOG_TRIVIAL(error) << "Shader Program linking failed:\n"
            << log;
    }

    // no longer needed once linked to program
    glDeleteShader(v);
    glDeleteShader(f);
}

shader::~shader() {
    glDeleteProgram(m_id);
}

constexpr const char* get_shadertype_name(int type)
{
    switch (type)
    {
    case(GL_VERTEX_SHADER):
        return "Vertex";

    case(GL_FRAGMENT_SHADER):
        return "Fragment";

    case(GL_GEOMETRY_SHADER):
        return "Geometry";

    default:
        return "Unkown";
    }
}

void shader::use() const {
    glUseProgram(m_id);
}

void shader::set_bool(const std::string& name, bool val) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)val);
}

void shader::set_int(const std::string& name, int val) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), val);
}

void shader::set_float(const std::string& name, float val) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), val);
}

void shader::set_vec2(const std::string& name, const glm::vec2& val) const {
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(val));
}

void shader::set_vec3(const std::string& name, const glm::vec3& val) const {
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(val));
}

void shader::set_vec4(const std::string& name, const glm::vec4& val) const {
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(val));
}

void shader::set_mat4x4(const std::string& name, const glm::mat4x4& val) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

uint32_t shader::compile_shader(const char* src, int type)
{
    uint32_t s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    int success = -1;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        const uint32_t LOG_LEN = 1024;
        char log[LOG_LEN];

        glGetShaderInfoLog(s, LOG_LEN, NULL, log);

        BOOST_LOG_TRIVIAL(error) << get_shadertype_name(type) << " Shader compilation failed:\n"
            << log;
    }

    return s;
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
