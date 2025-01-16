#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <cstdint>

#ifdef __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#endif

namespace tavern {

class window
{
public:

    window(const std::string& name);
    ~window();

    bool init(uint16_t w, uint16_t h, int flags = 0);
    bool update();
    void clean();

    bool open() const {
        return m_open;
    }

    void set_title(const std::string& name);

private:
    bool m_open = false;

    SDL_Window* m_window = NULL;

#ifdef USE_OPENGL
    SDL_GLContext m_glcontext = {};
#endif // USE_OPENGL

    std::string m_name;

}; /* end of class window */

} /* end of namespace tavern */

#endif /* end of define guard WINDOW_H */
