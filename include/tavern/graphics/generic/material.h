#ifndef MATERIAL_H
#define MATERIAL_H

#include <cstdint>
#include <memory>
#include <string>

#include <glm/vec3.hpp>

namespace tavern::graphics::generic {

template <typename tex_type>
struct material
{
    std::string name;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    std::shared_ptr<tex_type> albedo;

    // currently unused, but part of base .mtl spec
    glm::vec3 transmission;

    uint8_t illum; // see https://paulbourke.net/dataformats/mtl/ for description of illumination models
    float dissolve; // ad-hoc transparency w/out accounting for thickness or spectral character;
}; /* end of struct material */

} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard MATERIAL_H */
