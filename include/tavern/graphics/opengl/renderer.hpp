#ifndef OPENGL_RENDERER_HPP
#define OPENGL_RENDERER_HPP
#ifdef USE_OPENGL

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

    bool pre_window_init() override {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

#ifndef NDEBUG
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // NDEBUG

        return true;
    }

    bool init(window& window) override;
    void clean() override;

    void set_viewport_size(const glm::ivec2& view_size) override {
        glViewport(0, 0, view_size.x, view_size.y);
    }

    void clear() override;
    void render(ecs::registry& registry) override;
    void swap_buffer(window& wnd) override;

private:

    SDL_GLContext m_glcontext = NULL;

}; /* end of class opengl_renderer final : public a_renderer */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_HPP */
