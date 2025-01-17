#include "tavern/window.h"

#include <boost/log/trivial.hpp>

namespace tavern {

window::window(const std::string& name) :
    m_name(name)
{}

window::~window() {
    clean();
    SDL_Quit();

    BOOST_LOG_TRIVIAL(trace) << "Shutdown SDL";
}

bool window::init(const maths::vector2i& size, int flags) {

    if (open())
        return true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize SDL:\n"
            << SDL_GetError();
        return false;
    }

    flags |= SDL_WINDOW_RENDERER;

    if (!graphics::sdl::pre_window_init())
        return false;

    m_window = SDL_CreateWindow(
        m_name.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        size.X, size.Y, flags
    );

    if (!m_window) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create SDL Window:\n"
            << SDL_GetError();
        return false;
    }

    if (!m_renderer.post_window_init(m_window))
        return false;

    // need to set for get_size to work
    m_open = true;
    maths::vector2i view_size = get_size();
    m_renderer.set_viewport_size(view_size);

    BOOST_LOG_TRIVIAL(trace) << "Created SDL Window: Width = " << view_size.X << ", Height = " << view_size.Y;
    return m_open;
}

bool window::update() {

    if (!open())
        return false;

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type)
        {
        case(SDL_QUIT):
            BOOST_LOG_TRIVIAL(trace) << "Recieved shutdown message";
            m_open = false;
            return false;

        case(SDL_WINDOWEVENT):
            handle_window_event(e.window);
            break;

        default:
            break;
        }
    }

    return true;
}

void window::clean() {

    if (!m_window)
        return;

    m_renderer.clean();
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

void window::handle_window_event(const SDL_WindowEvent& e) {

    switch(e.event)
    {
    case(SDL_WINDOWEVENT_RESIZED):
        BOOST_LOG_TRIVIAL(trace) << "Window resized: X = " << e.data1 << ", Y = " << e.data2;
        m_renderer.set_viewport_size(maths::vector2i(e.data1, e.data2));
        break;

    default:
        break;
    }
}

} /* end of namespace tavern */
