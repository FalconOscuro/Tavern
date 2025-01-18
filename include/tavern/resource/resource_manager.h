#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>

#include "shader_manager.hpp"
#include "mesh_manager.h"

namespace tavern {

class resource_manager
{
public:

    static resource_manager& get() {
        static resource_manager mgr;
        return mgr;
    }

    resource::shader_manager::resource_ptr load_shader(const std::string& path) {
        return m_shaders.load(path);
    }

    resource::mesh_manager::resource_ptr load_mesh(const std::string& path) {
        return m_meshes.load(path);
    }

private:

    resource_manager()
    {}

    resource::shader_manager m_shaders;
    resource::mesh_manager   m_meshes;

}; /* end of class resource_manager */

} /* end of namespace tavern */

#endif /* end of define guard RESOURCE_MANAGER_H */
