#ifdef USE_OPENGL

#include "tavern/graphics/opengl/mesh.h"

#include "GL/glew.h"

namespace tavern::graphics::opengl {

// could use vector?
mesh::mesh(const maths::vector3f* verticies, const uint32_t vertex_count) {

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(maths::vector3f) * vertex_count, verticies, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(0);
}

mesh::~mesh() {
    glDeleteBuffers(1, &m_vertex_buffer);
}

void mesh::use() const {
    glBindVertexArray(m_id);
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
