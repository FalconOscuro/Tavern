#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <ryml.hpp>

#include "tavern/resource/shader_manager.hpp"
#include "tavern/resource/mesh_manager.h"
#include "tavern/resource/texture2d_manager.h"
#include "tavern/resource/material_manager.h"

namespace tavern {

class resource_manager
{
public:

    static resource_manager& singleton() {
        static resource_manager mgr;
        return mgr;
    }

    resource::shader_manager    shaders;
    resource::mesh_manager      meshes;
    resource::texture2d_manager tex2ds;
    resource::material_manager  materials;

    // get from templated managed type?
    // makes easier w/ templating resource r/w
    // change managers to pure templated instances, instead of using inheritance
    // following similar structure to ecs component pools

private:

    explicit resource_manager()
    {}
}; /* end of class resource_manager */

} /* end of namespace tavern */

#endif /* end of define guard RESOURCE_MANAGER_H */
