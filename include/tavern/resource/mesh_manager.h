#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include "resource_type_manager.hpp"
#include "tavern/graphics/mesh.h"

namespace tavern::resource {

class mesh_manager : public resource_type_manager<graphics::mesh>
{
protected:

    graphics::mesh* load_new(file::ifile* file) override;

}; /* end of class mesh_manager : public resource_type_manager<graphics::vertex_array> */

} /* end of namespace tavern::resource */

#endif /* end of define guard MESH_MANAGER_H */
