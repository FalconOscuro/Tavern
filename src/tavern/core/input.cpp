#include "tavern/core/input.h"

namespace tavern {

void input::handle_key_event(const SDL_KeyboardEvent& e)
{
    // ignore repeat key presses
    if (e.repeat)
        return;

    key& key_state = m_keys[e.keysym.scancode];

    if (e.state == SDL_PRESSED) {
        //BOOST_LOG_TRIVIAL(trace) << "Key pressed: " << SDL_GetKeyName(e.keysym.sym);
        key_state.state = key::PRESSED;
        key_state.press_start = SDL_GetTicks64();
    }

    else {
        //BOOST_LOG_TRIVIAL(trace) << "Key released: " << SDL_GetKeyName(e.keysym.sym) << ", after " << SDL_GetTicks64() - key_state.press_start << "ms";
        key_state.state = key::RELEASED;
    }
}

void input::update()
{
    for (uint32_t i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (m_keys[i].state == key::RELEASED)
            m_keys[i].state = key::NONE;
    }
}

} /* end of namespace tavern */
