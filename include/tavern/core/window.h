#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <glm/vec2.hpp>

#include "../platform/sdl.h"

namespace tavern {

class window
{
public:

    window(){}

    // prevent copy
    window(const window&) = delete;
    void operator=(const window&) = delete;

    window(const std::string& name):
        m_name(name)
    {}

    ~window() {
        shutdown();
    }

    bool init(const glm::ivec2& size, int flags = 0);
    void shutdown();

    bool open() const {
        return m_open;
    }

    bool compare_window_id(const uint32_t win_id) {
        // check win_id not zero as indicates error;
        return SDL_GetWindowID(m_window) == win_id && win_id;
    }

    void set_title(const std::string& name);

    glm::ivec2 get_size() const {
        glm::ivec2 size = glm::ivec2(0);

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

    bool m_open = false;
    SDL_Window* m_window = NULL;

    std::string m_name;

}; /* end of class window */

} /* end of namespace tavern */

#endif /* end of define guard WINDOW_H */
