#ifndef MATERIAL_H
#define MATERIAL_H

#include <cstdint>
#include <string>

#include "../../maths/vector3.hpp"

namespace tavern::graphics::generic {

struct material
{
    std::string name;

    maths::vector3f ambient;
    maths::vector3f diffuse;
    maths::vector3f specular;

    // currently unused, but part of base .mtl spec
    maths::vector3f transmission;

    uint8_t illum; // see https://paulbourke.net/dataformats/mtl/ for description of illumination models
    float dissolve; // ad-hoc transparency w/out accounting for thickness or spectral character;
}; /* end of struct material */

} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard MATERIAL_H */
