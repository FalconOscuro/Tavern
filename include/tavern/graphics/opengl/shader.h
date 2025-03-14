#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#ifdef USE_OPENGL

#include <cstdint>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../material.h"

namespace tavern::graphics::opengl {

class shader
{
public:

    // currently invidual shaders are discarded and only program is kept, could re-do so shaders are kept and formed into programs?
    shader(const char* vertex_src, const char* fragment_src);
    ~shader();

    void use() const;
    static void disable();

    // utility functions for accessing shader uniforms
    // NOTE: All assume shader has already been set active
    void set_bool(const std::string& name, bool val);
    void set_int(const std::string& name, int val);
    void set_float(const std::string& name, float val);

    inline void set_vec2(const std::string& name, const glm::vec2& val) {
        set_vec2_arr(name, glm::value_ptr(val));
    }

    inline void set_vec3(const std::string& name, const glm::vec3& val) {
        set_vec3_arr(name, glm::value_ptr(val));
    }

    inline void set_vec4(const std::string& name, const glm::vec4& val) {
        set_vec4_arr(name, glm::value_ptr(val));
    }

    void set_mat4x4(const std::string& name, const glm::mat4x4& val);

    // NOTE: May run into issues with glm? unsure
    void set_vec2_arr(const std::string& name, const float* val, const size_t num = 1);
    void set_vec3_arr(const std::string& name, const float* val, const size_t num = 1);
    void set_vec4_arr(const std::string& name, const float* val, const size_t num = 1);

    void set_transform(const glm::mat4& t);
    void set_material(const material_resource& m);

private:

    int get_attribute_location(const std::string& name);

    uint32_t compile_shader(const char* src, int type);

    uint32_t m_id;

    // store shader uniform names and associated locations to reduce queries to Opengl api
    // WARNING: Thread safety?
    std::unordered_map<std::string, int> m_attribute_map;
}; /* end of class shader */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard SHADER_H */
