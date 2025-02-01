#ifdef USE_OPENGL

#include "tavern/graphics/opengl/shader.h"

#include <GL/glew.h>

#include <boost/log/trivial.hpp>

namespace tavern::graphics::opengl {

struct shader_loc
{
    static constexpr int model = 0;
    static constexpr int albedo_tex_num = 0;
    static constexpr int metallic_roughness_tex_num = 1;
    static constexpr int normal_tex_num = 2;
    static constexpr int ambient_occlusion_tex_num = 3;
    static constexpr int emissive_tex_num = 4;
};

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

void shader::set_bool(const std::string& name, bool val) {
    glUniform1i(get_attribute_location(name), (int)val);
}

void shader::set_int(const std::string& name, int val) {
    glUniform1i(get_attribute_location(name), val);
}

void shader::set_float(const std::string& name, float val) {
    glUniform1f(get_attribute_location(name), val);
}

void shader::set_vec2(const std::string& name, const glm::vec2& val) {
    glUniform2fv(get_attribute_location(name), 1, glm::value_ptr(val));
}

void shader::set_vec3(const std::string& name, const glm::vec3& val) {
    glUniform3fv(get_attribute_location(name), 1, glm::value_ptr(val));
}

void shader::set_vec4(const std::string& name, const glm::vec4& val) {
    glUniform4fv(get_attribute_location(name), 1, glm::value_ptr(val));
}

void shader::set_mat4x4(const std::string& name, const glm::mat4x4& val) {
    glUniformMatrix4fv(get_attribute_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

void shader::set_transform(const glm::mat4& t) {
    glUniformMatrix4fv(shader_loc::model, 1, GL_FALSE, glm::value_ptr(t));
}

void shader::set_material(const material& m)
{
    // check if textures set and useable
    const bool use_albedo_tex = m.albedo_tex != nullptr;
    const bool use_metallic_roughness_tex = m.metallic_roughness_tex != nullptr;
    const bool use_normal_tex = m.normal_tex != nullptr;
    const bool use_ambient_occlusion_tex = m.ambient_occlusion_tex != nullptr;
    const bool use_emissive_tex = m.emissive_tex != nullptr;

    set_bool("mat.use_albedo_tex", use_albedo_tex);
    set_bool("mat.use_metallic_roughness_tex", use_metallic_roughness_tex);
    set_bool("mat.use_normal_tex", use_normal_tex);
    set_bool("mat.use_ambient_occlusion_tex", use_ambient_occlusion_tex);
    set_bool("mat.use_emissive_tex", use_emissive_tex);

    if (!use_albedo_tex)
        set_vec3("mat.albedo", m.albedo);
    else {
        glActiveTexture(GL_TEXTURE0 + shader_loc::albedo_tex_num);
        set_int("mat.albedo_tex", shader_loc::albedo_tex_num);
        m.albedo_tex->use();
    }

    if (!use_metallic_roughness_tex) {
        set_float("mat.metallic", m.metallic);
        set_float("mat.roughness", m.roughness);
    }
    else {
        glActiveTexture(GL_TEXTURE0 + shader_loc::metallic_roughness_tex_num);
        set_int("mat.metallic_roughness_tex", shader_loc::metallic_roughness_tex_num);
        m.metallic_roughness_tex->use();
    }

    if (!use_ambient_occlusion_tex)
        set_float("mat.ambient_occlusion", m.ambient_occlusion);
    else {
        glActiveTexture(GL_TEXTURE0 + shader_loc::ambient_occlusion_tex_num);
        set_int("mat.ambient_occlusion_tex", shader_loc::ambient_occlusion_tex_num);
        m.ambient_occlusion_tex->use();
    }

    if (!use_emissive_tex)
        set_vec3("mat.emissive", m.emissive);
    else {
        glActiveTexture(GL_TEXTURE0 + shader_loc::emissive_tex_num);
        set_int("mat.emissive_tex", shader_loc::emissive_tex_num);
        m.emissive_tex->use();
    }
}

GLint shader::get_attribute_location(const std::string& name)
{
    if (m_attribute_map.count(name))
        return m_attribute_map[name];

    GLint loc = glGetUniformLocation(m_id, name.c_str());

    if (loc >= 0)
        m_attribute_map[name] = loc;

    else
        BOOST_LOG_TRIVIAL(error) << "Error whilst finding uniform \"" << name << "\" from shader program id(" << m_id << ")";

    return loc;
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
