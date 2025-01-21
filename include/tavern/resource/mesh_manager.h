#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include "resource_type_manager.hpp"
#include "../graphics/graphics.h"

namespace tavern::resource {

class mesh_manager : public resource_type_manager<graphics::render_mesh>
{
protected:

    graphics::render_mesh* load_new(const std::string& path) override;

}; /* end of class mesh_manager : public resource_type_manager<graphics::vertex_array> */

} /* end of namespace tavern::resource */

#endif /* end of define guard MESH_MANAGER_H */
