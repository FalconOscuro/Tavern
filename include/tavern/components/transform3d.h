#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ecs/ecs.h>

namespace tavern {

namespace system {

class scene_tree;

} /* end of namespace system */

namespace component {

struct transform
{
    friend class tavern::system::scene_tree;

    glm::mat4 local;

    // Refer to parent entity transform by ID
    // If tombstone, no parent
    // Need tombstone retrievable at compile time
    ecs::entity_type parent = UINT32_MAX;

    const glm::mat4& get_global() const {
        return m_global;
    }

private:

    glm::mat4 m_global;
};

}} /* end of namespace tavern::component */

#endif /* end of define guard TRANSFORM3D_H */
