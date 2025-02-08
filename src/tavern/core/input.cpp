#include "tavern/core/input.h"
#include <SDL_mouse.h>

namespace tavern {

void input::handle_key_event(const SDL_KeyboardEvent& e)
{
    // ignore repeat key presses
    if (e.repeat)
        return;

    update_key(e.keysym.scancode, e.state);
}

void input::handle_mouse_event(const SDL_MouseButtonEvent& e)
{
    if (e.button > SDL_BUTTON_X2) {
        BOOST_LOG_TRIVIAL(error) << "Unrecognized mouse button, out of range!";
        return;
    }

    update_key(e.button + SDL_NUM_SCANCODES - 1, e.state);
}

void input::update()
{
    for (uint32_t i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (m_keys[i].state == key::RELEASED)
            m_keys[i].state = key::NONE;
    }
}

} /* end of namespace tavern */
