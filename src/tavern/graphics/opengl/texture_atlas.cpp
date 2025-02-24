#ifdef USE_OPENGL

#include "tavern/graphics/opengl/texture_atlas.h"

#include <GL/glew.h>

namespace tavern::graphics::opengl
{

texture_atlas::texture_atlas(const unsigned char* data, const glm::ivec3& size)
{
    glGenTextures(1, &m_atlas);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_atlas);

    // allocate storage
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, size.x, size.y, size.z);

    // upload pixel data
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,                      // mipmap level
        GL_RGBA8,               // gpu texel format
        size.x,                 // individual texture width
        size.y,                 // individual texture height
        size.z,                 // number of textures in atlas
        0,                      // border
        GL_RGB,                 // cpu texel format
        GL_UNSIGNED_BYTE,
        data
    );

    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    // texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

texture_atlas::~texture_atlas() {
    glDeleteTextures(1, &m_atlas);
}

void texture_atlas::use() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_atlas);
}

} /* end of namespace tavern::graphics::opengl
 */

#endif // USE_OPENGL
