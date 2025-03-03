#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>

#include <ecs/ecs.h>

namespace tavern::gui {

// is this necessary?
struct container
{
    std::vector<ecs::entity_type> children;

    enum {
        STACK = 0, // hack to create effective sub-viewport, maybe un-necessary?
        HORIZONTAL,
        VERTICAL,
    } strategy;
}; /* end of class struct */

} /* end of namespace tavern::gui */

#endif /* end of define guard CONTAINER_H */
