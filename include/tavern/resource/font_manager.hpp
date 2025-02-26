#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include "resource_type_manager.hpp"
#include "tavern/graphics/font.h"

namespace tavern::resource {

class font_manager : public resource_type_manager<graphics::font>
{
protected:

    graphics::font* load_new(const std::string& path) override {
        (void)path;
        return nullptr;
    }
}; /* end of class font_manager : public resource_type_manager<graphics::font> */

} /* end of namespace tavern::resource */

#endif /* end of define guard FONT_MANAGER_HPP */
