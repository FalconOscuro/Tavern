#ifdef USE_OPENGL

#include "tavern/graphics/opengl/mesh.h"

namespace tavern::graphics::opengl {

mesh::mesh(const std::vector<vertex>& vertices, const std::vector<face>& faces):
    mesh(vertices.data(), faces.data(), vertices.size(), faces.size())
{}

mesh::mesh(const vertex* vertices, const face* faces, const size_t num_vertices, const size_t num_faces):
    m_num_faces(num_faces)
{
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_index_buffer);

    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);

    // Copy vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * num_vertices, vertices, GL_STATIC_DRAW);

    // copy index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face) * num_faces, faces, GL_STATIC_DRAW);

    // locations for vertex attribute data
    // position
    glEnableVertexAttribArray(vertex_attributes::POSITION);
    glVertexAttribPointer(vertex_attributes::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

    // normal
    glEnableVertexAttribArray(vertex_attributes::NORMAL);
    glVertexAttribPointer(vertex_attributes::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));

    // tangent
    glEnableVertexAttribArray(vertex_attributes::TANGENT);
    glVertexAttribPointer(vertex_attributes::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tangent));

    // bitangent
    glEnableVertexAttribArray(vertex_attributes::BI_TANGENT);
    glVertexAttribPointer(vertex_attributes::BI_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, bitangent));

    // texture coordinates
    glEnableVertexAttribArray(vertex_attributes::TEXTURE_COORDINATE);
    glVertexAttribPointer(vertex_attributes::TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texcoord));

    glBindVertexArray(0);
}

mesh::~mesh() {
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_index_buffer);
    glDeleteVertexArrays(1, &m_id);
}

void mesh::draw() const {
    glBindVertexArray(m_id);
    glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} /* end of namespace tavern::graphics::opengl */

#endif
