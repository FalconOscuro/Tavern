#ifndef GUI_H
#define GUI_H

#include <ecs/ecs.h>

#include "tavern/gui/bounds.h"

namespace tavern::system {

class gui_sys
{
public:

    void update();

private:

    void update_containers();
    void update_screen_coords();

    void update_bound(ecs::entity_type eid, gui::bounds& bound, const glm::ivec2& container_pos, const glm::ivec2& container_size, ecs::registry& reg);

}; /* end of class gui_sys */

} /* end of namespace tavern::system */

#endif /* end of define guard GUI_H */
