#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <cstdint>

#include "platform/sdl.h"

#include "maths/vector2.hpp"
#include "graphics/renderer.h"

namespace tavern {

class window
{
public:

    window(const std::string& name);
    ~window();

    bool init(const maths::vector2i& size, int flags = 0);
    bool update();
    void clean();

    void draw_frame() {
        if (!open())
            return;

        m_renderer.swap_buffer(m_window);
        m_renderer.clear();
    }

    bool open() const {
        return m_open;
    }

    void set_title(const std::string& name);

    maths::vector2i get_size() const {
        return open() ? graphics::sdl::get_viewport_size(m_window) : maths::vector2i();
    }

private:

    void handle_window_event(const SDL_WindowEvent& e);

    bool m_open = false;
    SDL_Window* m_window = NULL;

    renderer m_renderer;

    std::string m_name;

}; /* end of class window */

} /* end of namespace tavern */

#endif /* end of define guard WINDOW_H */
