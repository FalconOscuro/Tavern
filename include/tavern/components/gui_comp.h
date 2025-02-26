#ifndef GUI_COMP_H
#define GUI_COMP_H

#include <memory>

#include "tavern/gui/control.h"

namespace tavern::component {

struct gui_comp
{

    std::unique_ptr<gui::control> control;
}; /* end of struct gui_comp */

} /* end of namespace tavern::component */

#endif /* end of define guard GUI_COMP_H */
