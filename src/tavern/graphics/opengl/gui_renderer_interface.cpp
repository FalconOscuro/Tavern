#ifdef USE_OPENGL

#include "tavern/graphics/opengl/gui_render_interface.h"

namespace tavern::graphics::opengl {

Rml::CompiledGeometryHandle gui_render_interface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
{
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
