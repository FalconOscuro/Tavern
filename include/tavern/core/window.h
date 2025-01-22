#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <glm/vec2.hpp>

#include "../platform/sdl.h"

#include "../graphics/graphics.h"

namespace tavern {

class window
{
public:

    window(const std::string& name);
    ~window();

    bool init(const glm::ivec2& size, int flags = 0);
    bool update(graphics::renderer& renderer);
    void clean();

    bool open() const {
        return m_open;
    }

    void set_title(const std::string& name);

    glm::ivec2 get_size() const {
        glm::ivec2 size;

        if (!open())
            return size;

        SDL_GetDrawableSize(m_window, &size.x, &size.y);
        return size;
    }

    operator SDL_Window*&() {
        return m_window;
    }

    operator SDL_Window*() const {
        return m_window;
    }

private:

    void handle_window_event(const SDL_WindowEvent& e, graphics::renderer& renderer);

    bool m_open = false;
    SDL_Window* m_window = NULL;

    std::string m_name;

}; /* end of class window */

} /* end of namespace tavern */

#endif /* end of define guard WINDOW_H */
