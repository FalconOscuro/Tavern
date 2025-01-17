#ifdef USE_OPENGL

#include "tavern/graphics/opengl/vertex_array.h"

#include "GL/glew.h"

namespace tavern::graphics::opengl {

vertex_array::vertex_array(maths::vector3f verticies[], uint32_t vertex_count) {

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(maths::vector3f) * vertex_count, verticies, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(0);
}

vertex_array::~vertex_array() {
    glDeleteBuffers(1, &m_vertex_buffer);
}

void vertex_array::use() const {
    glBindVertexArray(m_id);
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
