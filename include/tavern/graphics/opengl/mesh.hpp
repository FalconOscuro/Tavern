#ifndef OPENGL_MESH_HPP
#define OPENGL_MESH_HPP
#include <istream>
#ifdef USE_OPENGL

#include <cstdint>
#include <memory>
#include <vector>

#include <GL/glew.h>

#include "../material.h"
#include "../vertex.h"

namespace tavern::graphics::opengl {

class mesh
{
public:

    mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<material>& mat):
        mat(mat), m_vertex_count(vertices.size())
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
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)sizeof(glm::vec3));

        // tangent
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3) * 2));

        // bitangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3) * 3));

        // texture coordinates
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3) * 4));
    }

    ~mesh() {
        glDeleteVertexArrays(1, &m_id);
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_index_buffer);
    }

    mesh& draw() {
        glBindVertexArray(m_id);
        glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);

        return *this;
    }

    std::shared_ptr<material> mat;

private:

    uint32_t m_vertex_count;
    uint32_t m_vertex_buffer;
    uint32_t m_index_buffer;

    uint32_t m_id;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_HPP */
