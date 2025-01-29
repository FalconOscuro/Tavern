#ifndef OPENGL_RENDERER_HPP
#define OPENGL_RENDERER_HPP
#ifdef USE_OPENGL

#include <memory>

#include <GL/glew.h>

#include "../generic/renderer.h"
#include "shader.h"

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
        m_aspect_ratio = (float)view_size.x / (float)view_size.y;
        glViewport(0, 0, view_size.x, view_size.y);
    }

    void clear() override;
    void render(ecs::registry& registry) override;
    void swap_buffer(window& wnd) override;

private:

    SDL_GLContext m_glcontext = NULL;

    ecs::entity_type m_camera = UINT32_MAX;
    float m_aspect_ratio = 1.f;

    std::shared_ptr<shader> m_default_shader;

}; /* end of class opengl_renderer final : public a_renderer */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_HPP */
