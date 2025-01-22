#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#ifdef USE_OPENGL

#include <cstdint>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace tavern::graphics::opengl {

class shader
{
public:

    shader(const char* vertex_src, const char* fragment_src);
    ~shader();

    void use() const;

    // utility functions for accessing shader uniforms
    void set_bool(const std::string& name, bool val) const;
    void set_int(const std::string& name, int val) const;
    void set_float(const std::string& name, float val) const;

    void set_vec2(const std::string& name, const glm::vec2& val) const;
    void set_vec3(const std::string& name, const glm::vec3& val) const;
    void set_vec4(const std::string& name, const glm::vec4& val) const;
    void set_mat4x4(const std::string& name, const glm::mat4x4& val) const;

private:

    uint32_t compile_shader(const char* src, int type);

    uint32_t m_id;
}; /* end of class shader */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard SHADER_H */
