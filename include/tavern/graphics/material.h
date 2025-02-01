#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include <glm/vec3.hpp>

#include "texture2d.h"

namespace tavern::graphics {

struct material
{
    typedef std::shared_ptr<texture2d> tex_ptr;

    glm::vec3 albedo;
    float metallic;
    float roughness;
    float ambient_occlusion;
    glm::vec3 emissive;

    tex_ptr albedo_tex;
    tex_ptr metallic_roughness_tex;
    tex_ptr normal_tex;
    tex_ptr ambient_occlusion_tex;
    tex_ptr emissive_tex;
}; /* end of struct material */

} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard MATERIAL_H */
