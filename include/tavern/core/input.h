#ifndef INPUT_H
#define INPUT_H

#include <string>

#include <boost/log/trivial.hpp>

#include "../platform/sdl.h"

namespace tavern {

struct key
{
    enum : char {
        NONE,
        PRESSED,
        RELEASED
    } state = NONE;

    uint64_t press_start;

}; /* end of struct key */

class input
{
public:

    void update();
    void handle_key_event(const SDL_KeyboardEvent& e);
    void handle_mouse_event(const SDL_MouseButtonEvent& e);

    bool is_key_pressed(const std::string& key_name) const {
        return get_key(key_name).state == key::PRESSED;
    }

    bool is_key_released(const std::string& key_name) const {
        return get_key(key_name).state == key::RELEASED;
    }

    /*
     *  \return key hold time in ms, or 0 if key neither pressed nor released
     */
    uint64_t get_key_hold_time(const std::string& key_name) const {
        const key& k = get_key(key_name);

        if (k.state == key::NONE)
            return 0;

        return SDL_GetTicks64() - k.press_start;
    }

private:

    inline const key& get_key(const std::string& key_name) const {
        SDL_Scancode key_code = SDL_GetScancodeFromName(key_name.c_str());

        // unkown key
        if (!key_code)
            BOOST_LOG_TRIVIAL(error) << "Unkown key: " << key_name;

        return m_keys[key_code];
    }

    inline void update_key(const uint32_t id, const bool pressed) {
        // check out of bounds?
        key& key_state = m_keys[id];

        if (pressed) {
            key_state.state = key::PRESSED;
            key_state.press_start = SDL_GetTicks64();
        }

        else
            key_state.state = key::RELEASED;
    }

    // Store mouse buttons after keys
    // could save space by subtract 1 to ignore unknown?
    key m_keys[SDL_NUM_SCANCODES + SDL_BUTTON_X2];

}; /* end of class input */

} /* end of namespace tavern */

#endif /* end of define guard INPUT_H */
