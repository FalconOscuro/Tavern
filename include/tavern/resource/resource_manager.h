#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>

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

private:

    resource_manager()
    {}
}; /* end of class resource_manager */

} /* end of namespace tavern */

#endif /* end of define guard RESOURCE_MANAGER_H */
