#ifndef TAVERN_H
#define TAVERN_H

#include <ecs/ecs.h>

#include "core/window.h"
#include "core/input.h"
#include "graphics/renderer.h"

#include "resource/resource_manager.h"
#include "platform/sdl.h"

#include "systems/gui.h"
#include "systems/scene.h"

namespace tavern {

class tavern
{
public:

    // delete copy construction, should only be accessed through singleton
    tavern(const tavern&) = delete;
    void operator=(const tavern&) = delete;

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

    // these could be built in as singletons too
    [[nodiscard]] ecs::registry& get_registry() {
        return m_registry;
    }

    [[nodiscard]] system::scene& get_scene() {
        return m_scene;
    }

    [[nodiscard]] window& get_window() {
        return m_window;
    }

    [[nodiscard]] graphics::renderer& get_renderer() {
        return m_renderer;
    }

    [[nodiscard]] static tavern& singleton() {
        static tavern t;
        return t;
    }

private:

    explicit tavern(){}

    bool handle_events();
    void handle_window_event(const SDL_WindowEvent& e);

    bool m_ready = false;
    bool m_running = false;

    window m_window;
    input m_input;
    graphics::renderer m_renderer;

    system::gui_sys m_gui;
    // TODO: Should be integrated into physics system
    system::scene m_scene;

    ecs::registry m_registry;
}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard TAVERN_H */
