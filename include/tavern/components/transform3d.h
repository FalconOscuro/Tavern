#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ecs/ecs.h>

namespace tavern::component {

struct transform
{
    glm::mat4x4 matrix;

    // Refer to parent entity transform by ID
    // If tombstone, no parent
    // Need tombstone retrievable at compile time
    ecs::entity_type parent = UINT32_MAX;
};

} /* end of namespace tavern::component */

#endif /* end of define guard TRANSFORM3D_H */
