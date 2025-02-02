#ifndef VERTEX_H
#define VERTEX_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace tavern::graphics {

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
}; /* end of struct vertex */

} /* end of namespace tavern::graphics */

#endif /* end of define guard VERTEX_H */
