#ifndef ENTITY_NAME_H
#define ENTITY_NAME_H

#include <cstring>
#include <string>

#define ENTITY_NAME_MAX_LEN 124

namespace tavern::component {

// basic component to add a name identifier to an entity
struct entity_name
{
    entity_name() = default;

    entity_name(const char* s) {
        std::strncpy(m_name, s, ENTITY_NAME_MAX_LEN);
    }

    entity_name(const std::string& s):
        entity_name(s.data())
    {}

    ~entity_name() = default;

    entity_name& operator=(const char* s) {
        std::strncpy(m_name, s, ENTITY_NAME_MAX_LEN);
        return *this;
    }

    entity_name& operator=(const std::string& s) {
        std::strncpy(m_name, s.data(), ENTITY_NAME_MAX_LEN);
        return *this;
    }

    const char* get() const {
        return m_name;
    }

    char* operator*() {
        return m_name;
    }

private:

    // final byte always reserved for nullchar
    char m_name[ENTITY_NAME_MAX_LEN + 1]{'\0'};
}; /* end of struct entity_name */

} /* end of namespace tavern::component */

#endif /* end of define guard ENTITY_NAME_H */
