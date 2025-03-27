#ifndef COMPONENT_TRANSFORM_H
#define COMPONENT_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ecs/ecs.h>

namespace glm {

} /* end of namespace glm */

namespace tavern {

class scene;

namespace component {

struct transform
{
    friend class tavern::scene;

    transform() = default;
    transform(const glm::mat4& local, const ecs::entity_type parent = ecs::entity_type(-1));

    ~transform() = default;

    glm::vec3 get_local_translate() const;
    glm::vec3 get_local_scale() const;
    glm::quat get_local_quaternion() const;

    inline glm::vec3 get_local_euler() const {
        return glm::eulerAngles(get_local_quaternion());
    }

    glm::vec3 get_global_translate() const;
    glm::vec3 get_global_scale() const;
    glm::quat get_global_quaternion() const;

    inline glm::vec3 get_global_euler() const {
        return glm::eulerAngles(get_global_quaternion());
    }

    glm::mat4 local = glm::mat4(1.f);

    // Refer to parent entity transform by ID
    // If tombstone, no parent
    // Need tombstone retrievable at compile time
    ecs::entity_type parent = ecs::entity_type(-1);

    inline const glm::mat4& get_global() const {
        return m_global;
    }

private:

    glm::mat4 m_global = glm::mat4(1.f);
}; /* end of struct transform */

}} /* end of namespace tavern::component */

#endif /* end of define guard COMPONENT_TRANSFORM_H */
