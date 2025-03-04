#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H
#ifdef USE_OPENGL

#include <cstdint>
#include <vector>

#include <GL/glew.h>

#include "shader.h"
#include "../material.h"
#include "../vertex.h"

namespace tavern::graphics::opengl {

// generic mesh?
class mesh
{
public:

    mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, const material_resource& mat);
    
    ~mesh();

    void draw(shader& s) const;

    material_resource mat;

private:

    uint32_t m_index_count;
    uint32_t m_vertex_buffer;
    uint32_t m_index_buffer;

    uint32_t m_id;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_H */
