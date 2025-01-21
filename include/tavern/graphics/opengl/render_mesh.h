#ifndef OPENGL_MESH_ARRAY_H
#define OPENGL_MESH_ARRAY_H
#ifdef USE_OPENGL

#include <cstdint>
#include <vector>

namespace tavern::graphics::opengl {

class render_mesh
{
public:

    render_mesh(const std::vector<float>& vertices, const std::vector<float>& texcoords);

    ~render_mesh();

    void use() const;

private:

    uint32_t m_vertex_buffer;
    uint32_t m_id;

    uint32_t m_vertex_count;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_ARRAY_H */
