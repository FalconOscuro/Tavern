#include "tavern/tavern.h"

namespace tavern {

tavern::tavern():
    m_window("Tavern")
{}

tavern::~tavern() {
    clean();
}

bool tavern::init() {
    m_ready = m_window.init(maths::vector2i(800, 600));
    return m_ready;
}

void tavern::run() {

    if (!ready())
        return;

    while (m_window.open()) {

        m_window.update();
        m_window.draw_frame();
    }
}

void tavern::clean() {
    m_ready = false;
    m_window.clean();
}

} /* end of namespace tavern */
