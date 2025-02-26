#ifdef USE_OPENGL
#include "tavern/graphics/opengl/font.h"

#include <GL/glew.h>

#include <glm/mat3x3.hpp>

namespace tavern::graphics::opengl {

font::font(const ttf2bmp::bmp_font& bmp_font):
    m_atlas(
        bmp_font.data,
        glm::ivec3(bmp_font.frame_width, bmp_font.frame_height, bmp_font.character_map.size())
) {
    m_font = bmp_font;
    // nullify data in copied char map, data is not needed and should be discarded
    m_font.data = nullptr;

    // create basic vertex info useable with all indices
    const float vertices[] = {
        0.f, 0.f,                                               0.f, 0.f,
        0.f, (float)m_font.frame_height,                        0.f, 1.f,
        (float)m_font.frame_width, 0.f,                         1.f, 0.f,

        0.f, (float)m_font.frame_height,                        0.f, 1.f,
        (float)m_font.frame_width, 0.f,                         1.f, 0.f,
        (float)m_font.frame_width, (float)m_font.frame_height,  1.f, 1.f
    };

    glGenBuffers(1, &m_vbo);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, vertices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // texture coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

font::~font() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

// text colour?
void font::draw_text(const std::string& text, shader& s, const glm::vec2& pos, const float scale) const
{
    s.use();
    glActiveTexture(GL_TEXTURE0);
    m_atlas.use();

    glm::vec3 offset;
    glBindVertexArray(m_vao);

    // iterate characters in string
    for (auto c = text.begin(); c != text.end(); ++c)
    {
        // newline
        if (*c == '\n') {
            offset.x = 0;
            offset.y += m_font.line_height;
            continue;
        }

        auto found = m_font.character_map.find(*c);

        // should assert nullchar always exists
        if (found == m_font.character_map.end())
            found = m_font.character_map.begin();

        const glm::mat4 transf =
            glm::translate(
                glm::scale(
                    glm::translate(glm::mat4(1.0), offset),
                    glm::vec3(scale)
                ), 
                glm::vec3(pos, 0)
        );

        s.set_int("glyph_num", found->first);
        s.set_transform(transf);

        offset.x += found->second.advance;

        // could use GL_QUADS
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec2 font::get_text_size(const std::string& text, const float scale) const
{
    glm::vec2 size;

    for (auto c = text.begin(); c != text.end(); ++c)
    {
        if (*c == '\n') {
            // if \n is last character then no need for new line
            if (c != text.end() - 1)
                size.y += m_font.line_height;

            continue;
        }

        auto glyph = m_font.character_map.find(*c);

        if (glyph == m_font.character_map.end())
            glyph = m_font.character_map.begin();

        // may not be fully accurate
        size.x += glyph->second.advance;
    }

    return size * scale;
}

} /* namespace tavern::graphics */

#endif // USE_OPENGL
