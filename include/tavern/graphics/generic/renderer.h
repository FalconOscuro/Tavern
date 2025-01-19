#ifndef GENERIC_RENDERER_H
#define GENERIC_RENDERER_H

#include <ecs/ecs.h>

#include "../../platform/sdl.h"
#include "../../maths/vector2.hpp"

namespace tavern {

class window;

namespace graphics::generic {

class renderer
{
public:
    virtual bool pre_window_init() = 0;
    virtual bool init(window& window) = 0;
    virtual void clean() = 0;

    virtual void set_viewport_size(const maths::vector2i& view_size) = 0;

    virtual void clear() = 0;

    virtual void render(ecs::registry& registry) = 0;

    virtual void swap_buffer(window& wnd) = 0;
}; /* end of class a_renderer */

}} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard GENERIC_RENDERER_H */
