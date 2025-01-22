#ifndef OPENGL_TEXTURE2D_HPP
#define OPENGL_TEXTURE2D_HPP
#ifdef USE_OPENGL

#include <GL/glew.h>

#include <glm/vec3.hpp>

namespace tavern::graphics::opengl {

class texture2d
{
public:

    texture2d(const unsigned char* data, const glm::ivec3& size)
    {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // wrapping and filtering for mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D, 0, 
            GL_RGB,
            size.x, size.y, 0,
            size.z == 4 ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    ~texture2d() {
        glDeleteTextures(1, &m_texture);
    }

    texture2d& use() {
        glBindTexture(GL_TEXTURE_2D, m_texture);
        return *this;
    }

private:

    unsigned int m_texture;
}; /* end of class texture2d */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_TEXTURE2D_HPP */
