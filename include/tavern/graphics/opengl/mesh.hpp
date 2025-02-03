#ifndef OPENGL_MESH_HPP
#define OPENGL_MESH_HPP
#ifdef USE_OPENGL

#include <cstdint>
#include <memory>
#include <vector>

#include <GL/glew.h>

#include "shader.h"
#include "../material.h"
#include "../vertex.h"

namespace tavern::graphics::opengl {

class mesh
{
public:

    mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<material>& mat):
        mat(mat), m_index_count(indices.size())
    {
        glGenBuffers(1, &m_vertex_buffer);
        glGenBuffers(1, &m_index_buffer);

        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);

        // Copy vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // copy index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

        // locations for vertex attribute data
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

        // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));

        // tangent
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tangent));

        // bitangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, bi_tangent));

        // texture coordinates
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texture_coordinates));
    }

    ~mesh() {
        glDeleteVertexArrays(1, &m_id);
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_index_buffer);
    }

    void draw(shader& s) {
        if (mat)
            s.set_material(*mat.get());

        glBindVertexArray(m_id);
        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    std::shared_ptr<material> mat;

private:

    uint32_t m_index_count;
    uint32_t m_vertex_buffer;
    uint32_t m_index_buffer;

    uint32_t m_id;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_HPP */
