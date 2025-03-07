#include "tavern/core/gui.h"

#include <RmlUi/Debugger.h>

namespace tavern {

bool gui::init()
{
    Rml::SetRenderInterface(&m_render_interface);

    return Rml::Initialise();
}

void gui::shutdown() {
    Rml::Shutdown();
}

} /* end of namespace tavern */
