#include "tavern/components/transform.h"

namespace tavern::component {

inline glm::vec3 extract_tf_translate(const glm::mat4& m) {
    return m[3];
}

inline glm::vec3 extract_tf_scale(const glm::mat4& m)
{
    glm::vec3 scale;

    for (size_t i = 0; i < 3; ++i)
        scale[i] = length(glm::vec3(m[0][i], m[1][i], m[2][i]));

    return scale;
}

inline glm::quat extract_tf_quaternion(const glm::mat4& m)
{
    const glm::vec3 scale = extract_tf_scale(m);

    glm::mat3 rm = m;
    for (size_t i = 0; i < 3; ++i)
        for (size_t j = 0; j < 3; ++j)
            rm[j][i] /= scale[i];

    return quat_cast(rm);
}

transform::transform(const glm::mat4& local, const ecs::entity_type parent):
    local(local), parent(parent)
{}

glm::vec3 transform::get_local_translate() const {
    return extract_tf_translate(local);
}

glm::vec3 transform::get_local_scale() const {
    return extract_tf_scale(local);
}

glm::quat transform::get_local_quaternion() const {
    return extract_tf_quaternion(local);
}

glm::vec3 transform::get_global_translate() const {
    return extract_tf_translate(m_global);
}

glm::vec3 transform::get_global_scale() const {
    return extract_tf_scale(m_global);
}

glm::quat transform::get_global_quaternion() const {
    return extract_tf_quaternion(m_global);
}

} /* end of namespace tavern::component */
