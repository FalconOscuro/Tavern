#ifndef CAMERA_H
#define CAMERA_H

namespace tavern::component {

struct camera
{
    float fov;
    float near;
    float far;

    bool active;
}; /* end of struct camera */

} /* end of namespace tavern::component */

#endif /* end of define guard CAMERA_H */
