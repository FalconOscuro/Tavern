
#include "tavern/core/engine.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>

#include "tavern/core/file_system.h"
#include "tavern/core/input.h"
#include "tavern/core/scene.h"
#include "tavern/core/window.h"
#include "tavern/graphics/renderer.h"

#include "tavern/resource/resource_manager.h"

namespace tavern {

bool engine::init(const uint16_t width, const uint16_t height, const std::string& name)
{
    BOOST_LOG_TRIVIAL(trace) << "Starting engine initialization...";

    // ensure resource manager created
    (void)resource_manager::get();

    window& w = window::singleton();
    graphics::renderer& r = graphics::renderer::singleton();

    w.set_title(name);

    m_ready =
           file_system::singleton().init()
        && r.pre_window_init()
        && w.init(glm::ivec2(width, height))
        && r.init(w.get_wnd(), w.get_size());

    BOOST_LOG_TRIVIAL(trace) << "Engine initialization complete";

    return m_ready;
}

void engine::run() {

    if (!ready() || m_running)
        return;

    m_running = true;

    input& s_input                 = input::singleton();
    scene& s_scene                 = scene::singleton();
    window& s_window               = window::singleton();
    graphics::renderer& s_renderer = graphics::renderer::singleton();

    BOOST_LOG_TRIVIAL(trace) << "Entering main loop";
    while (handle_events()) {

        // system updates
        s_scene.update();
        s_renderer.update();

        // rendering
        s_renderer.clear();
        s_renderer.render();

        s_renderer.gui_draw();

        // present frame
        s_renderer.swap_buffer(s_window.get_wnd());

        // inaccurate on 1st frame
        s_input.update();
    }
    BOOST_LOG_TRIVIAL(trace)  << "Exited main engine loop";
    m_running = false;
}

void engine::shutdown() {
    if (!m_ready)
        return;

    m_ready = false;
    file_system::singleton().unmount_all();
    scene::singleton().shutdown();
    graphics::renderer::singleton().shutdown();
    window::singleton().shutdown();
}

// should be part of window class?
bool engine::handle_events()
{
    SDL_Event e;

    window& s_window = window::singleton();
    input& s_input   = input::singleton();

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
            if (s_window.compare_window_id(e.key.windowID))
                s_input.handle_key_event(e.key);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (s_window.compare_window_id(e.button.windowID))
                s_input.handle_mouse_event(e.button);
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
        graphics::renderer::singleton().set_viewport_size(window::singleton().get_size());
        //m_gui.resize(m_window.get_size());
        break;

    default:
        break;
    }
}

} /* end of namespace tavern */
