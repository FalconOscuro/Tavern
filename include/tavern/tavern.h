#ifndef TAVERN_H
#define TAVERN_H

#include <ecs/ecs.h>

#include "core/window.h"

namespace tavern {

class tavern
{
public:

    tavern();
    ~tavern();

    bool init();
    void run();
    void clean();

    bool ready() const {
        return m_ready;
    }

private:

    bool m_ready = false;

    window m_window;

    ecs::registry m_registry;
}; /* end of class tavern */

} /* end of namespace tavern */

#endif /* end of define guard TAVERN_H */
