#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#ifdef USE_OPENGL

#include <cstdint>
#include <string>

namespace tavern::graphics::opengl {

class shader
{
public:

    shader(const char* vertex_src, const char* fragment_src);
    ~shader();

    void use() const;

    // utility functions for accessing shader uniforms

private:

    uint32_t compile_shader(const char* src, int type);

    uint32_t m_id;
}; /* end of class shader */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard SHADER_H */
