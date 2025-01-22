#ifdef USE_OPENGL

#include "tavern/graphics/opengl/render_mesh.h"

#include "GL/glew.h"

namespace tavern::graphics::opengl {

// could use vector?
render_mesh::render_mesh(const std::vector<float>& verticies, const std::vector<float>& texcoords):
    m_vertex_count(verticies.size() / 3)
{

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticies.size(), verticies.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(0);
}

render_mesh::~render_mesh() {
    glDeleteBuffers(1, &m_vertex_buffer);
}

void render_mesh::use() const {
    glBindVertexArray(m_id);
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
