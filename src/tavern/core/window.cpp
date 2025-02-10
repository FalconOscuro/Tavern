#include "tavern/core/window.h"

#include <boost/log/trivial.hpp>


namespace tavern {

window::~window() {
    clean();
    SDL_Quit();

    BOOST_LOG_TRIVIAL(trace) << "Shutdown SDL";
}

bool window::init(const glm::ivec2& size, int flags) {

    if (open())
        return true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize SDL:\n"
            << SDL_GetError();
        return false;
    }

    flags |= SDL_WINDOW_RENDERER;

    m_window = SDL_CreateWindow(
        m_name.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        size.x, size.y, flags
    );

    if (!m_window) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create SDL Window:\n"
            << SDL_GetError();
        return false;
    }

    // Must set before get_size
    m_open = true;
    glm::ivec2 view_size = get_size();
    BOOST_LOG_TRIVIAL(trace) << "Created SDL Window: Width = " << view_size.x << ", Height = " << view_size.y;
    return true;
}

void window::clean() {

    if (!m_window)
        return;

    SDL_DestroyWindow(m_window);
    m_window = NULL;
    m_open = false;

    BOOST_LOG_TRIVIAL(trace) << "Closed SDL Window";
}

void window::set_title(const std::string& name) {
    m_name = name;

    if (!open())
        return;

    SDL_SetWindowTitle(m_window, m_name.c_str());
}

} /* end of namespace tavern */
