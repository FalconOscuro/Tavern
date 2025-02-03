#include "tavern/tavern.h"

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

    if (!ready())
        return;

    BOOST_LOG_TRIVIAL(trace) << "Entering main loop";
    while (m_window.open()) {

        m_window.update(m_renderer);

        m_scene_tree.update(m_registry);

        m_renderer.render(m_registry);
        m_renderer.swap_buffer(m_window);
    }
    BOOST_LOG_TRIVIAL(trace)  << "Exited main engine loop";
}

void tavern::clean() {
    m_ready = false;
    m_registry.destroy_all();
    m_renderer.clean();
    m_window.clean();
}

} /* end of namespace tavern */
