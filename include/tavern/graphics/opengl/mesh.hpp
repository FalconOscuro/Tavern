#ifndef OPENGL_MESH_HPP
#define OPENGL_MESH_HPP
#ifdef USE_OPENGL

#include <cstdint>
#include <vector>

#include <GL/glew.h>

namespace tavern::graphics::opengl {

class mesh
{
public:

    mesh(const std::vector<float>& vertices, const std::vector<float>& texcoords):
        m_vertex_count(vertices.size() / 3)
    {
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_tex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_tex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, m_tex_buffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * vertices.size()));

        glEnableVertexAttribArray(0);
    }

    ~mesh() {
        glDeleteVertexArrays(1, &m_id);
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_tex_buffer);
    }

    mesh& use() {
        glBindVertexArray(m_id);
        glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);

        return *this;
    }

private:

    uint32_t m_vertex_count;
    uint32_t m_vertex_buffer;
    uint32_t m_tex_buffer;

    uint32_t m_id;
}; /* end of class mesh */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_MESH_HPP */
