#ifndef OPENGL_RENDERER_HPP
#define OPENGL_RENDERER_HPP
#ifdef USE_OPENGL

#include <memory>

#include <GL/glew.h>

#include "../generic/renderer.h"
#include "../shader.h"

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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

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

    void render(ecs::registry& registry) override;
    void swap_buffer(window& wnd) override;

private:

    void imgui_draw();

    template<typename T>
    static uint32_t create_uniform_buffer(const T* data = nullptr) {
        return create_uniform_buffer(data, sizeof(T));
    }

    static uint32_t create_uniform_buffer(const void* data, const uint32_t size) {
        uint32_t buffer;
        glCreateBuffers(1, &buffer);
        glNamedBufferStorage(buffer, size, data, GL_DYNAMIC_STORAGE_BIT);
        return buffer;
    }

    SDL_GLContext m_glcontext = NULL;

    uint32_t m_camera_ub;

    ecs::entity_type m_camera = UINT32_MAX;
    float m_aspect_ratio = 1.f;

    shader_resource m_default_shader;

}; /* end of class opengl_renderer final : public a_renderer */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_HPP */
