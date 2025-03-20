#ifndef ENTITY_NAME_H
#define ENTITY_NAME_H

#include <string>

namespace tavern::component {

// basic component to add a name identifier to an entity
struct entity_name
{
    std::string name;
}; /* end of struct entity_name */

} /* end of namespace tavern::component */

#endif /* end of define guard ENTITY_NAME_H */
