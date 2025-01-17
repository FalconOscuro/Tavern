#ifndef GENERIC_RENDERER_H
#define GENERIC_RENDERER_H

#include "../../platform/sdl.h"
#include "../../maths/vector2.hpp"

namespace tavern::graphics::generic {

class renderer
{
public:

    virtual bool post_window_init(SDL_Window* wnd) = 0;
    virtual void update() = 0;
    virtual void clean() = 0;

    virtual void set_viewport_size(const maths::vector2i& view_size) = 0;

    virtual void swap_buffer(SDL_Window* wnd) = 0;
    virtual void clear() = 0;

}; /* end of class a_renderer */

namespace sdl {

// Do not have meaningful effect on renderer class, so separated out to avoid confusion
bool pre_window_init();
maths::vector2i get_viewport_size(SDL_Window* wnd);

} /* end of namespace sdl */

} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard GENERIC_RENDERER_H */
