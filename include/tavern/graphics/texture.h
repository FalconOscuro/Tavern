#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl/texture2d.h"
#include "opengl/texture_atlas.h"

#include "../resource/resource_ptr.hpp"

namespace tavern::graphics {

typedef RENDERER_NS::texture2d texture2d;
typedef resource::resource_ptr<texture2d> texture2d_resource;

typedef RENDERER_NS::texture_atlas texture_atlas;

} /* end of namespace tavern::graphics */

#endif /* end of define guard TEXTURE2D_H */
