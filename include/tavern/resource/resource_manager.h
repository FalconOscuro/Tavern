#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>

#include "shader_manager.hpp"

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


private:

    resource_manager()
    {}

    resource::shader_manager m_shaders;

}; /* end of class resource_manager */

} /* end of namespace tavern */

#endif /* end of define guard RESOURCE_MANAGER_H */
