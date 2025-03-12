#ifndef OPENGL_TEXTURE_ATLAS_H
#define OPENGL_TEXTURE_ATLAS_H
#ifdef USE_OPENGL

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace tavern::graphics::opengl
{

// inheritance/relationship with texture2d?
class texture_atlas
{
public:

    // size refers to size of individual texture on atlas, with z component being total number of textures
    texture_atlas(const unsigned char* data, const glm::ivec3& size);

    ~texture_atlas();

    void use() const;
    static void disable();

private:

    unsigned int m_atlas;

}; /* end of class texture_atlas */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_TEXTURE_ATLAS_H */
