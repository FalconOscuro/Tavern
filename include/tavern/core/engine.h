#ifndef ENGINE_H
#define ENGINE_H

#include <string>

#include "tavern/platform/sdl.h"

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

    [[nodiscard]] static engine& singleton() {
        static engine instance;
        return instance;
    }

private:
    engine() = default;

    bool handle_events();
    void handle_window_event(const SDL_WindowEvent& e);

    bool m_ready = false;
    bool m_running = false;
}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard ENGINE_H */
