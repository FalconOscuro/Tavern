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
}

// text colour?
void font::draw_text(const std::string& text, shader& s, const glm::vec2& pos, const float scale) const
{
    s.use();
    m_atlas.use();

    glm::vec3 offset;

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
            found = m_font.character_map.find('\0');

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
    }
}

} /* namespace tavern::graphics */

#endif // USE_OPENGL
