#include "tavern/tavern.h"

#include "tavern/resource/resource_manager.h"

namespace tavern {

tavern::tavern():
    m_window("Tavern")
{}

tavern::~tavern() {
    clean();
}

bool tavern::init() {
    resource_manager::get();
    return m_ready =
           m_renderer.pre_window_init()
        && m_window.init(maths::vector2i(800, 600))
        && m_renderer.init(m_window);
}

void tavern::run() {

    if (!ready())
        return;

    while (m_window.open()) {

        m_window.update();

        m_renderer.clear();
        m_renderer.render(m_registry);
        m_renderer.swap_buffer(m_window);
    }
}

void tavern::clean() {
    m_ready = false;
    m_renderer.clean();
    m_window.clean();
}

} /* end of namespace tavern */
