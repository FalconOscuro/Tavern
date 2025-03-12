#ifndef TAVERN_H
#define TAVERN_H

#include "core/window.h"
#include "core/input.h"
#include "core/gui.h"
#include "core/scene.h"

#include "graphics/renderer.h"

#include "resource/resource_manager.h"
#include "platform/sdl.h"


namespace tavern {

class engine
{
public:

    engine(){}

    // delete copy construction, should only be accessed through singleton
    engine(const engine&) = delete;
    void operator=(const engine&) = delete;

    ~engine() {
        shutdown();
    }

    bool init(const uint16_t width, const uint16_t height, const std::string& name);
    void run();
    void shutdown();

    bool ready() const {
        return m_ready;
    }

    bool running() const {
        return m_running;
    }

    [[nodiscard]] scene& get_scene() {
        return m_scene;
    }

    [[nodiscard]] window& get_window() {
        return m_window;
    }

    [[nodiscard]] graphics::renderer& get_renderer() {
        return m_renderer;
    }

    [[nodiscard]] gui& get_gui() {
        return m_gui;
    }

private:

    bool handle_events();
    void handle_window_event(const SDL_WindowEvent& e);

    bool m_ready = false;
    bool m_running = false;

    window m_window;
    input m_input;
    gui m_gui;
    scene m_scene;

    graphics::renderer m_renderer;

}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard TAVERN_H */
