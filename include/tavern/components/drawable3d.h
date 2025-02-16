#ifndef DRAWABLE3D_H
#define DRAWABLE3D_H

#include "../graphics/mesh.h"
#include "../graphics/shader.h"

#include <ryml.hpp>

namespace tavern::component {

struct drawable3d
{

    drawable3d()
    {}

    drawable3d(graphics::mesh_resource& mesh):
        mesh(mesh)
    {}

    graphics::mesh_resource mesh;
    graphics::shader_resource shader;
};

bool read(const ryml::ConstNodeRef& n, drawable3d* val);
void write(ryml::NodeRef* n, const drawable3d& val);

} /* namespace tavern::component */

#endif /* end of include guard: DRAWABLE3D_H */
