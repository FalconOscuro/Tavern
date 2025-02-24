#ifdef USE_OPENGL

#include "tavern/graphics/font.h"

namespace tavern::graphics {

font::font(const ttf2bmp::bmp_font& bmp_font):
    m_texture(bmp_font.data, glm::ivec3(bmp_font.frame_width, bmp_font.get_buffer_height(), 1))
{
    m_font = bmp_font;
    // nullify data in copied char map, data is not needed and should be discarded
    m_font.data = nullptr;
}

// pos as arg?
// text colour?
void font::draw_text(const std::string& text) const
{
}

} /* namespace tavern::graphics */

#endif // USE_OPENGL
