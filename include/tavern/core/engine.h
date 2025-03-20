#ifndef ENGINE_H
#define ENGINE_H

#include "window.h"
#include "input.h"
#include "scene.h"

#include "tavern/graphics/renderer.h"

namespace tavern {

class engine
{
public:
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

    [[nodiscard]] static engine& singleton() {
        static engine instance;
        return instance;
    }

private:
    engine(){}

    bool handle_events();
    void handle_window_event(const SDL_WindowEvent& e);

    bool m_ready = false;
    bool m_running = false;

    window m_window;
    input m_input;
    scene m_scene;

    graphics::renderer m_renderer;
}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard ENGINE_H */
