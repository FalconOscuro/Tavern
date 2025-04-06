#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <vector>

#include "tavern/graphics/material.h"
#include "tavern/graphics/mesh.h"
#include "tavern/graphics/shader.h"

namespace tavern::component {

struct skinned_mesh
{
    skinned_mesh() = default;
    skinned_mesh(const graphics::mesh_resource& mesh, const graphics::material_resource& material):
        mesh(mesh), material(material)
    {}

    ~skinned_mesh() = default;

    graphics::mesh_resource mesh;
    graphics::material_resource material;

    operator bool() const {
        return mesh;
    }
}; /* end of struct skinned_mesh */

struct render_mesh
{
    render_mesh() = default;
    ~render_mesh() = default;

    std::vector<skinned_mesh> meshes;
};

} /* namespace tavern::component */

#endif /* end of include guard: RENDER_MESH_H */
