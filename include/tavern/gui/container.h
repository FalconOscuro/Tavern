#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>

#include <ecs/ecs.h>

namespace tavern::gui {

class container
{
    std::vector<ecs::entity_type> children;
}; /* end of class container */

} /* end of namespace tavern::gui */

#endif /* end of define guard CONTAINER_H */
