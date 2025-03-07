#ifndef GUI_H
#define GUI_H

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

    bool init();
    void shutdown();

private:

    Rml::Context* m_context = nullptr;

    graphics::gui_renderer_interface m_render_interface;

}; /* end of class gui */

} /* end of namespace tavern */

#endif /* end of define guard GUI_H */
