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
    m_ticks = SDL_GetTicks64();

    for (uint32_t i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (m_keys[i].state == key::RELEASED)
            m_keys[i].state = key::NONE;
    }
}

uint64_t input::get_key_hold_time(const std::string& key_name) const
{
        const key& k = get_key(key_name);

        if (k.state == key::NONE)
            return 0;

        return m_ticks - k.press_start;
}

glm::ivec2 input::get_mouse_pos() const
{
    glm::ivec2 pos;
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
}

const key& input::get_key(const std::string& key_name) const
{
    SDL_Scancode key_code = SDL_GetScancodeFromName(key_name.c_str());

    // unkown key
    if (!key_code)
        BOOST_LOG_TRIVIAL(error) << "Unkown key: " << key_name;

    return m_keys[key_code];
}

void input::update_key(const uint32_t id, const bool pressed)
{
        // check out of bounds?
        key& key_state = m_keys[id];

        if (pressed) {
            key_state.state = key::PRESSED;
            key_state.press_start = m_ticks;
        }

        else
            key_state.state = key::RELEASED;
}

} /* end of namespace tavern */
