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

uint32_t shader::compile_shader(const char* fragment_src, int type)
{
    uint32_t s = glCreateShader(type);
    glShaderSource(s, 1, &fragment_src, NULL);
    glCompileShader(s);

    int success = -1;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);

    if (!success) {
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
