#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H
#ifdef USE_OPENGL

#define SDL_WINDOW_RENDERER SDL_WINDOW_OPENGL

#include <GL/glew.h>

#include "a_renderer.h"

namespace tavern {

namespace graphics {

class opengl_renderer : public a_renderer
{
public:

    opengl_renderer() = default;
    ~opengl_renderer() {
        clean();
    }

    bool post_window_init(SDL_Window* wnd) override;
    void update() override;
    void clean() override;

    void set_viewport_size(const maths::vector2i& view_size) override {
        glViewport(0, 0, view_size.X, view_size.Y);
    }

private:

    SDL_GLContext m_glcontext = NULL;

}; /* end of class opengl_renderer final : public a_renderer */

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

} /* end of namespace graphics */

typedef graphics::opengl_renderer renderer;

} /* end of namespace tavern */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_H */
