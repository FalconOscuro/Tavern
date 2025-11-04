#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include "tavern/graphics/material.h"
#include "tavern/graphics/mesh.h"
#include "tavern/graphics/shader.h"

namespace tavern::component {

struct render_mesh
{
    render_mesh() = default;
    render_mesh(const graphics::mesh_resource& mesh, const graphics::material_resource& material):
        mesh(mesh), material(material)
    {}

    ~render_mesh() = default;

    graphics::mesh_resource mesh;
    graphics::material_resource material;

    // custom shaders?

    operator bool() const {
        return mesh;
    }
};

} /* namespace tavern::component */

#endif /* end of include guard: RENDER_MESH_H */
