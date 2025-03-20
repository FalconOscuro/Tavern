#ifndef GENERIC_RENDERER_H
#define GENERIC_RENDERER_H

#include <memory>
#include <string>
#include <unordered_map>

#include <glm/vec2.hpp>

#include <ecs/ecs.h>

#include "tavern/graphics/imgui_panel.h"

namespace tavern {

class window;

namespace graphics::generic {

class renderer
{
public:

    renderer() = default;
    virtual ~renderer() = default;

    virtual bool pre_window_init() = 0;
    virtual bool init(void* window, const glm::ivec2 size) = 0;

    inline void shutdown() {
        shutdown_gui();
        shutdown_renderer();
    }

    virtual void set_viewport_size(const glm::ivec2& view_size) = 0;

    virtual void update();

    virtual void clear() = 0;
    virtual void render() = 0;

    void gui_draw();

    virtual void swap_buffer(void* window) = 0;

    ecs::entity_type get_active_camera() const {
        return m_camera;
    }

    bool camera_exists() const;

    // NOTE: Once gui_layer is added, object is owned by the renderer
    // deletion should only be done after calling remove_layer or through delete layer
    // overwrites if layer already exists
    // discards if layer is nullptr NOTE: This means nullptr will not override existing layers, slighlty counterintuitive
    void add_gui_layer(const std::string& layer_name, imgui_panel* layer);
    imgui_panel* get_layer(const std::string& layer_name) const;

    // Remove layer ownership from renderer without deleting
    [[nodiscard]] imgui_panel* remove_layer(const std::string& layer_name);

    inline void delete_layer(const std::string& layer_name) {
        delete remove_layer(layer_name);
    }

protected:

    virtual void ready_gui_draw() = 0;
    virtual void end_gui_draw() = 0;

    virtual void shutdown_renderer() = 0;

private:

    void shutdown_gui();

    void update_camera();
    void no_camera_warning() const;

    ecs::entity_type m_camera = 0;

    std::unordered_map<std::string, std::unique_ptr<imgui_panel>> m_gui_layers;
}; /* end of class a_renderer */

}} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard GENERIC_RENDERER_H */
