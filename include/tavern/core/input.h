#ifndef INPUT_H
#define INPUT_H

#include <string>

#include <boost/log/trivial.hpp>

#include <glm/ext/vector_int2.hpp>

#include "tavern/platform/sdl.h"

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

    ~input() = default;

    // prevent copy
    input(const input&) = delete;
    void operator=(const input&) = delete;

    void update();
    void handle_key_event(const SDL_KeyboardEvent& e);
    void handle_mouse_event(const SDL_MouseButtonEvent& e);

    inline bool is_key_pressed(const std::string& key_name) const {
        return get_key(key_name).state == key::PRESSED;
    }

    inline bool is_key_just_pressed(const std::string& key_name) const {
        return is_key_pressed(key_name) && get_key_hold_time(key_name) == 0;
    }

    inline bool is_key_released(const std::string& key_name) const {
        return get_key(key_name).state == key::RELEASED;
    }

    /*
     *  \return key hold time in ms, 0 if key neither pressed nor released or if key just pressed, used key_just pressed to check for this
     */
    uint64_t get_key_hold_time(const std::string& key_name) const;
    glm::ivec2 get_mouse_pos() const;

    [[nodiscard]] static input& singleton() {
        static input instance;
        return instance;
    }

private:

    input() = default;

    const key& get_key(const std::string& key_name) const;

    void update_key(const uint32_t id, const bool pressed);

    // Store mouse buttons after keys
    // could save space by subtract 1 to ignore unknown?
    key m_keys[SDL_NUM_SCANCODES + SDL_BUTTON_X2];

    uint64_t m_ticks = 0;

}; /* end of class input */

} /* end of namespace tavern */

#endif /* end of define guard INPUT_H */
