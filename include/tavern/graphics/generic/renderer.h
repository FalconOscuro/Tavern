#ifndef GENERIC_RENDERER_H
#define GENERIC_RENDERER_H

#include <glm/vec2.hpp>

#include <ecs/ecs.h>

namespace tavern {

class window;

namespace graphics::generic {

class renderer
{
public:
    virtual bool pre_window_init() = 0;
    virtual bool init(void* window, const glm::ivec2 size) = 0;
    virtual void shutdown() = 0;

    virtual void set_viewport_size(const glm::ivec2& view_size) = 0;

    virtual void update();

    virtual void clear() = 0;
    virtual void render() = 0;
    virtual void swap_buffer(void* window) = 0;

    ecs::entity_type get_active_camera() const {
        return m_camera;
    }

    bool camera_exists() const;

private:

    void update_camera();
    void no_camera_warning() const;

    ecs::entity_type m_camera = 0;
}; /* end of class a_renderer */

}} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard GENERIC_RENDERER_H */
