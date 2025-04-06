#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>

#include "texture.h"
#include "tavern/resource/resource_ptr.hpp"

namespace tavern::graphics {

struct material
{
    glm::vec3 albedo;
    float metallic;
    float roughness;
    float ambient_occlusion = 1;
    glm::vec3 emissive;

    texture2d_resource albedo_tex;
    texture2d_resource metallic_tex;
    texture2d_resource roughness_tex;
    texture2d_resource normal_tex;
    texture2d_resource ambient_occlusion_tex;
    texture2d_resource emissive_tex;
}; /* end of struct material */

typedef resource::resource_ptr<material> material_resource;

} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard MATERIAL_H */
