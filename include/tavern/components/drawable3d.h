#ifndef DRAWABLE3D_H
#define DRAWABLE3D_H

#include "../resource/shader_manager.hpp"
#include "../resource/mesh_manager.h"

namespace tavern::component {

struct drawable3d {

    drawable3d()
    {}

    drawable3d(std::shared_ptr<graphics::mesh>& mesh, std::shared_ptr<graphics::shader> shader = nullptr):
        mesh(mesh), shader(shader)
    {}

    resource::mesh_manager::resource_handle mesh;
    resource::shader_manager::resource_handle shader = nullptr;
};

} /* namespace tavern::component */

#endif /* end of include guard: DRAWABLE3D_H */
