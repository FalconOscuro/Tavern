#ifndef OPENGL_RENDERER_HPP
#define OPENGL_RENDERER_HPP
#ifdef USE_OPENGL

#define SDL_WINDOW_RENDERER SDL_WINDOW_OPENGL

#include <GL/glew.h>

#include "../generic/renderer.h"

namespace tavern::graphics::opengl {

class renderer : public generic::renderer
{
public:

    renderer() = default;
    ~renderer() {
        clean();
    }

    bool post_window_init(SDL_Window* wnd) override;
    void update() override;
    void clean() override;

    void set_viewport_size(const maths::vector2i& view_size) override {
        glViewport(0, 0, view_size.X, view_size.Y);
    }

    void swap_buffer(SDL_Window* wnd) override;
    void clear() override;

private:

    SDL_GLContext m_glcontext = NULL;

}; /* end of class opengl_renderer final : public a_renderer */

namespace sdl {

inline bool pre_window_init() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // NDEBUG

    return true;
}

inline maths::vector2i get_viewport_size(SDL_Window* wnd) {
    maths::vector2i size;
    SDL_GL_GetDrawableSize(wnd, &size.X, &size.Y);
    return size;
}

} /* end of namespace sdl */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_HPP */
