#ifndef COMPONENT_TRANSFORM_H
#define COMPONENT_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ryml.hpp>

#include <ecs/ecs.h>

namespace glm {

inline vec3 extract_tf_translate(const mat4& m) {
    return m[3];
}

inline vec3 extract_tf_scale(const mat4& m)
{
    vec3 scale;

    for (size_t i = 0; i < 3; ++i)
        scale[i] = vec3(m[i]).length();

    return scale;
}

inline quat extract_tf_quaternion(const mat4& m)
{
    const vec3 scale = extract_tf_scale(m);

    mat3 rm;
    for (size_t i = 0; i < 3; ++i)
        rm[i] = vec3(rm[i]) / scale[i];

    return quat_cast(rm);
}

inline vec3 extract_tf_euler(const mat4& m)
{
    const quat q = extract_tf_quaternion(m);
    return glm::eulerAngles(q);
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
        return glm::extract_tf_euler(m_global);
    }

    inline glm::vec3 get_local_euler() const {
        return glm::extract_tf_euler(local);
    }


    glm::mat4 local = glm::mat4(1.f);

    // Refer to parent entity transform by ID
    // If tombstone, no parent
    // Need tombstone retrievable at compile time
    ecs::entity_type parent = UINT32_MAX;

    inline const glm::mat4& get_global() const {
        return m_global;
    }

private:

    glm::mat4 m_global = glm::mat4(1.f);
}; /* end of struct transform */

// ryml read/write
bool read(const ryml::ConstNodeRef& n, transform* val);
void write(ryml::NodeRef* n, const transform& val);

}} /* end of namespace tavern::component */

namespace glm {
// NOTE: Move definition file?

// ryml read/write for mat4 and vec4 (coltype for mat4)
// requires definition in glm namespace
bool read(const ryml::ConstNodeRef& n, mat4* val);
void write(ryml::NodeRef* n, const mat4& val);

bool read(const ryml::ConstNodeRef& n, vec4* val);
void write(ryml::NodeRef* n, const vec4& val);

} /* namespace glm */

#endif /* end of define guard COMPONENT_TRANSFORM_H */
