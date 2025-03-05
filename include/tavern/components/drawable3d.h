#ifndef DRAWABLE3D_H
#define DRAWABLE3D_H

#include "tavern/graphics/material.h"
#include "tavern/graphics/mesh.h"
#include "tavern/graphics/shader.h"

#include <ryml.hpp>

namespace tavern::component {

struct drawable3d
{

    drawable3d()
    {}

    drawable3d(const graphics::mesh_resource& mesh, const graphics::material_resource& material):
        mesh(mesh), material(material)
    {}

    graphics::mesh_resource mesh;
    graphics::material_resource material;
    graphics::shader_resource shader;
};

bool read(const ryml::ConstNodeRef& n, drawable3d* val);
void write(ryml::NodeRef* n, const drawable3d& val);

} /* namespace tavern::component */

#endif /* end of include guard: DRAWABLE3D_H */
