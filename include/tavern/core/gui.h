#ifndef GUI_H
#define GUI_H

#include <glm/vec2.hpp>

#include <RmlUi/Core.h>

#include "tavern/graphics/gui_render_interface.h"

namespace tavern {

class gui
{
public:
    gui() {}

    ~gui() {
        shutdown();
    }

    // disable copy
    gui(const gui&) = delete;
    void operator=(const gui&) = delete;

    bool init(const glm::ivec2& size);
    void shutdown();

    void update();
    void render();

    void resize(const glm::ivec2& size);

    bool load_document(const char* path);

private:

    Rml::Context* m_context = nullptr;

    graphics::gui_renderer_interface m_render_interface;

    bool m_initialized = false;
    bool m_show_debug_info = false;

}; /* end of class gui */

} /* end of namespace tavern */

#endif /* end of define guard GUI_H */
