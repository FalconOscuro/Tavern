#include "tavern/gui/label.h"

namespace tavern::gui {

void label::draw() const
{
    // no font to use or no text to draw
    if (!font || !shader || text.empty())
        return;

    font->draw_text(text, *(shader.get()), glm::vec2(), size);
}

} /* end of namespace tavern::gui */
