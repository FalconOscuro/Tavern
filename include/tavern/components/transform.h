#ifndef COMPONENT_TRANSFORM_H
#define COMPONENT_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ecs/ecs.h>

namespace glm {

inline vec3 extract_tf_translate(const mat4& m) {
    return m[3];
}

inline vec3 extract_tf_scale(const mat4& m)
{
    vec3 scale;

    for (size_t i = 0; i < 3; ++i)
        scale[i] = length(vec3(m[0][i], m[1][i], m[2][i]));

    return scale;
}

inline quat extract_tf_quaternion(const mat4& m)
{
    const vec3 scale = extract_tf_scale(m);

    mat3 rm = m;
    for (size_t i = 0; i < 3; ++i)
        for (size_t j = 0; j < 3; ++j)
            rm[j][i] /= scale[i];

    return quat_cast(rm);
}

} /* end of namespace glm */

namespace tavern {

class scene;

namespace component {

struct transform
{
    friend class tavern::scene;

    transform()
    {}

    transform(const glm::mat4& local, const ecs::entity_type parent = UINT32_MAX):
        local(local), parent(parent)
    {}

    inline glm::vec3 get_global_translate() const {
        return glm::extract_tf_translate(m_global);
    }

    inline glm::vec3 get_local_translate() const {
        return glm::extract_tf_translate(local);
    }

    inline glm::vec3 get_global_scale() const {
        return glm::extract_tf_scale(m_global);
    }

    inline glm::vec3 get_local_scale() const {
        return glm::extract_tf_scale(local);
    }

    inline glm::quat get_global_quaternion() const {
        return glm::extract_tf_quaternion(m_global);
    }

    inline glm::quat get_local_quaternion() const {
        return glm::extract_tf_quaternion(local);
    }

    inline glm::vec3 get_global_euler() const {
        return glm::eulerAngles(get_global_quaternion());
    }

    inline glm::vec3 get_local_euler() const {
        return glm::eulerAngles(get_local_quaternion());
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
