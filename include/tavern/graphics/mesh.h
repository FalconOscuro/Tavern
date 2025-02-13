#ifndef GRAPHICS_MESH_H
#define GRAPHICS_MESH_H

#include "opengl/mesh.hpp"
#include "../resource/fwd.h"

namespace tavern::graphics {

typedef RENDERER_NS::mesh mesh;
typedef resource::resource_ptr<mesh> mesh_resource;

} /* end of namespace tavern::graphics */

#endif /* end of define guard GRAPHICS_MESH_H */
