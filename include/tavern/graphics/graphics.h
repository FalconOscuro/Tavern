#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef USE_OPENGL

#include "opengl/renderer.hpp"
#include "opengl/shader.h"
#include "opengl/mesh.h"

#define RENDERER_NS opengl

#endif // USE_OPENGL

namespace tavern::graphics {

typedef RENDERER_NS::renderer renderer;

typedef RENDERER_NS::mesh mesh;
typedef RENDERER_NS::shader shader;

} /* end of namespace tavern::graphics */

#endif /* end of define guard GRAPHICS_H */
