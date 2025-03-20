
#include "tavern/core/engine.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>

#include "tavern/resource/resource_manager.h"

namespace tavern {

bool engine::init(const uint16_t width, const uint16_t height, const std::string& name)
{
    BOOST_LOG_TRIVIAL(trace) << "Starting engine initialization...";

    // ensure resource manager created
    (void)resource_manager::get();
    m_window.set_title(name);

    m_ready =
           m_renderer.pre_window_init()
        && m_window.init(glm::ivec2(width, height))
        && m_renderer.init(m_window.get_wnd(), m_window.get_size());

    BOOST_LOG_TRIVIAL(trace) << "Engine initialization complete";

    return m_ready;
}

void engine::run() {

    if (!ready() || m_running)
        return;

    m_running = true;

    BOOST_LOG_TRIVIAL(trace) << "Entering main loop";
    while (handle_events()) {

        // system updates
        m_input.update();
        //m_gui.update();
        m_scene.update();
        m_renderer.update();

        // rendering
        m_renderer.clear();
        m_renderer.render();

        m_renderer.gui_draw();

        // present frame
        m_renderer.swap_buffer(m_window.get_wnd());
    }
    BOOST_LOG_TRIVIAL(trace)  << "Exited main engine loop";
    m_running = false;
}

void engine::shutdown() {
    if (!m_ready)
        return;

    m_ready = false;
    m_scene.shutdown();
    m_renderer.shutdown();
    m_window.shutdown();
}

// should be part of window class?
bool engine::handle_events()
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

void engine::handle_window_event(const SDL_WindowEvent& e)
{
    switch (e.event)
    {
    case SDL_WINDOWEVENT_RESIZED:
        BOOST_LOG_TRIVIAL(trace) << "Window resized: X = " << e.data1 << ", Y = " << e.data2;
        // use window get_size as drawable size may differ from actual window size
        m_renderer.set_viewport_size(m_window.get_size());
        //m_gui.resize(m_window.get_size());
        break;

    default:
        break;
    }
}

} /* end of namespace tavern */
