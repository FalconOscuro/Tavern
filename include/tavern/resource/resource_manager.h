#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <ryml.hpp>

#include "shader_manager.hpp"
#include "mesh_manager.h"
#include "texture2d_manager.h"
#include "material_manager.h"

namespace tavern {

class resource_manager
{
public:

    static resource_manager& get() {
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

namespace resource {

} /* namespace resource */

} /* end of namespace tavern */

#endif /* end of define guard RESOURCE_MANAGER_H */
