#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H
#ifdef USE_OPENGL

#include "a_renderer.h"

namespace tavern {

namespace graphics {

class opengl_renderer : public a_renderer
{
public:

    opengl_renderer() = default;
    ~opengl_renderer() = default;

    bool init() override;
    void update() override;

}; /* end of class opengl_renderer final : public a_renderer */

} /* end of namespace graphics */

typedef graphics::opengl_renderer renderer;

} /* end of namespace tavern */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_H */
