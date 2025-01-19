#ifndef TAVERN_H
#define TAVERN_H

#include <ecs/ecs.h>

#include "core/window.h"
#include "graphics/graphics.h"
#include "resource/resource_manager.h"

namespace tavern {

class tavern
{
public:

    tavern();
    ~tavern();

    bool init();
    void run();
    void clean();

    bool ready() const {
        return m_ready;
    }

private:

    bool m_ready = false;

    window m_window;
    graphics::renderer m_renderer;

    ecs::registry m_registry;
}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard TAVERN_H */
