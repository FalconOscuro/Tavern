#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H
#ifdef USE_OPENGL

#include "tavern/graphics/generic/mesh.h"

#include <vector>

#include <GL/glew.h>

#include "tavern/graphics/vertex.h"

namespace tavern::graphics::opengl {

class mesh final : generic::mesh
{
public:

    mesh(const vertex* vertices, const face* faces, const size_t num_vertices, const size_t num_faces);
    mesh(const std::vector<vertex>& vertices, const std::vector<face>& faces);

    // no copy
    mesh(const mesh&) = delete;
    void operator=(const mesh&) = delete;

    ~mesh();

    void draw() const override;

private:

    uint32_t m_num_faces;
    uint32_t m_vertex_buffer;
    uint32_t m_index_buffer;

    uint32_t m_id;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_H */
