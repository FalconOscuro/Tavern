#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>

#include <ryml.hpp>

namespace tavern::component {

struct camera
{
    camera()
    {}

    camera(float fov, float near, float far, bool active = true):
        fov(fov), near(near), far(far), active(active)
    {}

    glm::mat4 get_view_matrix(float aspect_ratio) const {
        return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    }

    float fov;
    float near;
    float far;

    bool active;
}; /* end of struct camera */

// ryml read/write
bool read(const ryml::ConstNodeRef& n, camera* val);
void write(ryml::NodeRef* n, const camera& val);

} /* end of namespace tavern::component */

#endif /* end of define guard CAMERA_H */
