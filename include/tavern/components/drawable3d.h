#ifndef DRAWABLE3D_H
#define DRAWABLE3D_H

#include "../resource/shader_manager.hpp"
#include "../resource/mesh_manager.h"

namespace tavern::component {

struct drawable3d {

    resource::mesh_manager::resource_ptr Mesh;
    resource::mesh_manager::resource_ptr Shader;
};

} /* namespace tavern::component */

#endif /* end of include guard: DRAWABLE3D_H */
