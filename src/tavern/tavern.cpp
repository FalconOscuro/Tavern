#include "tavern/tavern.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>

#include "tavern/resource/resource_manager.h"
#include "tavern/components/drawable3d.h"

namespace tavern {

tavern::tavern():
    m_window("Tavern")
{}

tavern::~tavern() {
    clean();
}

bool tavern::init() {
    resource_manager::get();
    m_ready =
           m_renderer.pre_window_init()
        && m_window.init(glm::ivec2(800, 600))
        && m_renderer.init(m_window);

    BOOST_LOG_TRIVIAL(trace) << "Engine initialization complete";

    return m_ready;
}

void tavern::run() {

    if (!ready() || m_running)
        return;

    m_running = true;

    BOOST_LOG_TRIVIAL(trace) << "Entering main loop";
    while (handle_events()) {

        m_scene.update(m_registry);

        m_renderer.render(m_registry);
        m_renderer.swap_buffer(m_window);

        m_input.update();
    }
    BOOST_LOG_TRIVIAL(trace)  << "Exited main engine loop";
    m_running = false;
}

void tavern::clean() {
    m_ready = false;
    m_registry.destroy_all();
    m_renderer.clean();
    m_window.clean();
}

bool tavern::handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);

        switch (e.type)
        {
        case SDL_QUIT:
            BOOST_LOG_TRIVIAL(trace) << "Recieved shutdown message";
            return m_running = false;

        case SDL_WINDOWEVENT:
            handle_window_event(e.window);
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (m_window.compare_window_id(e.key.windowID))
                m_input.handle_key_event(e.key);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (m_window.compare_window_id(e.button.windowID))
                m_input.handle_mouse_event(e.button);
            break;

        default:
            break;
        }
    }

    return m_running;
}

void tavern::handle_window_event(const SDL_WindowEvent& e)
{
    switch (e.event)
    {
    case SDL_WINDOWEVENT_RESIZED:
        BOOST_LOG_TRIVIAL(trace) << "Window resized: X = " << e.data1 << ", Y = " << e.data2;
        // use window get_size as drawable size may differ from actual window size
        m_renderer.set_viewport_size(m_window.get_size());
        break;

    default:
        break;
    }
}

} /* end of namespace tavern */
