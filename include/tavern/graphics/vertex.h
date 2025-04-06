#ifndef VERTEX_H
#define VERTEX_H

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace tavern::graphics {

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texcoord;
}; /* end of struct vertex */

struct face
{
    uint32_t indices[3];
}; /* end of struct face */

enum vertex_attributes {
    POSITION,
    NORMAL,
    TANGENT,
    BI_TANGENT,
    TEXTURE_COORDINATE,
    COLOUR
};

} /* end of namespace tavern::graphics */

#endif /* end of define guard VERTEX_H */
