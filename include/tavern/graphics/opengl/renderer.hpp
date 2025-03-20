#ifndef OPENGL_RENDERER_HPP
#define OPENGL_RENDERER_HPP
#ifdef USE_OPENGL

#include <GL/glew.h>

#include <ecs/ecs.h>

#include "tavern/graphics/generic/renderer.h"
#include "tavern/graphics/shader.h"

namespace tavern::graphics::opengl {

class renderer final : public generic::renderer
{
public:

    ~renderer() = default;

    // prevent copy
    renderer(const renderer&) = delete;
    void operator=(const renderer&) = delete;

    bool pre_window_init() override;

    bool init(void* window, const glm::ivec2 size) override;

    void set_viewport_size(const glm::ivec2& view_size) override {
        m_aspect_ratio = (float)view_size.x / (float)view_size.y;
        glViewport(0, 0, view_size.x, view_size.y);
    }

    void clear() override;
    void render() override;
    void swap_buffer(void* window) override;

    [[nodiscard]] static renderer& singleton() {
        static renderer instance;
        return instance;
    }

protected:

    void ready_gui_draw() override;
    void end_gui_draw() override;

    void shutdown_renderer() override;

private:

    renderer() = default;

    void render_geometry();

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

    void* m_glcontext = NULL;

    uint32_t m_camera_ub;

    float m_aspect_ratio = 1.f;

    shader_resource m_default_shader;

}; /* end of class opengl_renderer final : public a_renderer */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_RENDERER_HPP */
