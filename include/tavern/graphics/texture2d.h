#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "opengl/texture2d.hpp"
#include "../resource/fwd.h"

namespace tavern::graphics {

typedef RENDERER_NS::texture2d texture2d;
typedef resource::resource_ptr<texture2d> texture2d_resource;

} /* end of namespace tavern::graphics */

#endif /* end of define guard TEXTURE2D_H */
