#ifndef FONT_H
#define FONT_H

#include <string>

#include <ttf2bmp/ttf2bmp.h>

#include "texture2d.h"

namespace tavern::graphics {

class font
{
    font(const ttf2bmp::bmp_font& bmp_font);
    ~font() = default;

    void draw_text(const std::string& text) const;

private:

    ttf2bmp::bmp_font m_font;

    // doesn't need to be resource, texture should be unique to font
    texture2d m_texture;

}; /* end of class font */

} /* namespace tavern::graphics */

#endif /* end of include guard: FONT_H */
