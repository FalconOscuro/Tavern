#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>

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

    float fov = 120.f;
    float near = 0.001f;
    float far = 1000.f;

    bool active;
}; /* end of struct camera */

} /* end of namespace tavern::component */

#endif /* end of define guard CAMERA_H */
