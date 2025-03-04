#ifndef ANCHOR_H
#define ANCHOR_H

#include <glm/vec2.hpp>

namespace tavern::gui {

struct anchor
{

    // position in relative screen space coordinates 0-1
    glm::vec2 pos;

    enum {
        CENTER = 0,
        TOP = 1,
        BOTTOM = 1 << 1,
        LEFT = 1 << 2,
        RIGHT = 1 << 3,
    } point;

}; /* end of struct anchor */

} /* end of namespace tavern::gui */

#endif /* end of define guard ANCHOR_H */
