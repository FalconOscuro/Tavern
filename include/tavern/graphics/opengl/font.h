#ifndef OPENGL_FONT_H
#define OPENGL_FONT_H

#include <string>

#include <glm/vec2.hpp>

#include <ttf2bmp/ttf2bmp.h>

#include "shader.h"
#include "texture_atlas.h"

namespace tavern::graphics::opengl {

class font
{
public:
    font(const ttf2bmp::bmp_font& bmp_font);
    ~font();

    void draw_text(const std::string& text, shader& s, const glm::vec2& pos, const float scale) const;

    // get size for bounding box of rendered text
    glm::vec2 get_text_size(const std::string& text, const float scale = 1.f) const;

private:

    ttf2bmp::bmp_font m_font;

    // doesn't need to be resource, texture should be unique to font
    texture_atlas m_atlas;

    unsigned int m_vao, m_vbo;

}; /* end of class font */

} /* namespace tavern::graphics */

#endif /* end of include guard: OPENGL_FONT_H */
