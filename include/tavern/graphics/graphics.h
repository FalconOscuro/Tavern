#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef USE_OPENGL

#include "opengl/renderer.hpp"
#include "opengl/shader.h"
#include "opengl/render_mesh.h"
#include "opengl/texture2d.hpp"

#define RENDERER_NS opengl

#endif // USE_OPENGL

namespace tavern::graphics {

typedef RENDERER_NS::renderer renderer;

typedef RENDERER_NS::render_mesh render_mesh;
typedef RENDERER_NS::shader shader;
typedef RENDERER_NS::texture2d texture2d;

} /* end of namespace tavern::graphics */

#endif /* end of define guard GRAPHICS_H */
