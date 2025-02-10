#ifndef TAVERN_H
#define TAVERN_H

#include <ecs/ecs.h>

#include "core/window.h"
#include "core/input.h"
#include "graphics/renderer.h"

#include "resource/resource_manager.h"
#include "platform/sdl.h"

#include "systems/scene.h"

namespace tavern {

class tavern
{
public:

    tavern(){}
    ~tavern() {
        clean();
    }

    bool init(const uint16_t width, const uint16_t height, const std::string& name);
    void run();
    void clean();

    bool ready() const {
        return m_ready;
    }

    bool running() const {
        return m_running;
    }

    ecs::registry& get_registry() {
        return m_registry;
    }

    system::scene& get_scene() {
        return m_scene;
    }

private:

    bool handle_events();
    void handle_window_event(const SDL_WindowEvent& e);

    bool m_ready = false;
    bool m_running = false;

    window m_window;
    input m_input;
    graphics::renderer m_renderer;

    // TODO: Should be integrated into physics system
    system::scene m_scene;

    ecs::registry m_registry;
}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard TAVERN_H */
