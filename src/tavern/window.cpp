#include "tavern/window.h"

#include <boost/log/trivial.hpp>

namespace tavern {

window::window(const std::string& name) :
    m_name(name)
{}

window::~window() {
    clean();
    SDL_Quit();

    BOOST_LOG_TRIVIAL(info) << "Shutdown SDL";
}

bool window::init(uint16_t w, uint16_t h, int flags) {

    if (open())
        return true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize SDL:\n"
            << SDL_GetError();
        return false;
    }

#ifdef USE_OPENGL
    flags |= SDL_WINDOW_OPENGL;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // NDEBUG

#endif // USE_OPENGL

    m_window = SDL_CreateWindow(
        m_name.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h, flags
    );

    if (!m_window) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create SDL Window:\n"
            << SDL_GetError();
        return false;
    }

#ifdef USE_OPENGL
    // init opengl context
    m_glcontext = SDL_GL_CreateContext(m_window);

    if (!m_glcontext) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create OpenGL context:\n"
            << SDL_GetError();
        return false;
    }
#endif // USE_OPENGL

    BOOST_LOG_TRIVIAL(info) << "Created SDL Window";
    m_open = true;
    return true;
}

bool window::update() {

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type)
        {
        case(SDL_QUIT):
            BOOST_LOG_TRIVIAL(info) << "Recieved shutdown message";
            m_open = false;
            return false;

        default:
            break;
        }
    }

    return true;
}

void window::clean() {

    if (!m_window)
        return;

    SDL_DestroyWindow(m_window);
    m_window = NULL;
    m_open = false;

    BOOST_LOG_TRIVIAL(info) << "Closed SDL Window";
}

void window::set_title(const std::string& name) {
    m_name = name;

    if (!open())
        return;

    SDL_SetWindowTitle(m_window, m_name.c_str());
}

} /* end of namespace tavern */
