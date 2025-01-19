#ifndef OPENGL_MESH_ARRAY_H
#define OPENGL_MESH_ARRAY_H
#ifdef USE_OPENGL

#include <cstdint>

#include "../../maths/vector3.hpp"

namespace tavern::graphics::opengl {

class mesh
{
public:

    // could use std::vector<> instead of array?
    mesh(const maths::vector3f* vertices, const uint32_t vertex_count);

    ~mesh();

    void use() const;

private:

    uint32_t m_vertex_buffer;
    uint32_t m_id;

    uint32_t m_vertex_count;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_ARRAY_H */
