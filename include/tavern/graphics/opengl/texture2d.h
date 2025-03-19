#ifndef OPENGL_TEXTURE2D_H
#define OPENGL_TEXTURE2D_H
#ifdef USE_OPENGL

#include <glm/vec3.hpp>

namespace tavern::graphics::opengl
{

class texture2d final
{
public:

    texture2d(const unsigned char* data, const glm::ivec3& size);

    ~texture2d();

    void use(const int tex_num = 0) const;
    static void disable();

private:

    unsigned int m_texture;
}; /* end of class texture2d */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_TEXTURE2D_H */
