#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include <glm/vec4.hpp>

#include "tavern/graphics/font.h"

namespace tavern::gui {

struct text
{
    float size = 1.f;

    glm::vec4 colour;

    graphics::font font;
}; /* end of struct text */

} /* end of namespace tavern::gui */

#endif /* end of define guard GUI_TEXT_H */
