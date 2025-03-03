#ifndef BOUNDS_H
#define BOUNDS_H

#include <glm/vec2.hpp>

#include <ecs/ecs.h>

namespace tavern {

namespace system {

class gui;

} /* end of namespace system */

namespace gui {

struct bounds
{
    friend class system::gui;

    // pos & size measured in screen space coordinates?
    glm::vec2 pos;
    glm::vec2 size;

    int layer;

    enum {
        NONE = 0,

        TOP = 1,
        BOTTOM = 1 << 1,
        VERTICAL_CENTER = TOP | BOTTOM,

        LEFT = 1 << 2,
        RIGHT = 1 << 3,
        HORIZONTAL_CENTER = LEFT | RIGHT
    } anchor;

    enum {
        X_AUTO = 1,
        X_STRETCH = 1 << 1,
        X_STRETCH_PREFER_MIN = X_STRETCH | X_AUTO,

        Y_AUTO = 1 << 2,
        Y_STRETCH = 1 << 3,
        Y_STRETCH_PREFER_MIN = Y_STRETCH | Y_AUTO,

        AUTO = X_AUTO | Y_AUTO,
        STRETCH = X_STRETCH | Y_STRETCH,
        STRETCH_PREFER_MIN = STRETCH | AUTO,
    } sizing;

    ecs::entity_type get_parent() const {
        return m_parent;
    }

private:

    // controlled by system only
    // Helps ensure single parent only
    ecs::entity_type m_parent = UINT32_MAX;

}; /* end of struct bounds */

}} /* end of namespace tavern::gui */

#endif /* end of define guard BOUNDS_H */
