#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include "resource_type_manager.hpp"
#include "tavern/graphics/material.h"

#include <boost/log/trivial.hpp>

namespace tavern::resource {

class material_manager : public resource_type_manager<graphics::material>
{
protected:

    graphics::material* load_new(const std::string& path) override {
        (void)path;
        BOOST_LOG_TRIVIAL(error) << "loading materials from direct from file is currently not supported";

        return nullptr;
    }

}; /* end of class material_manager : public resource_type_manager<graphics::material> */

} /* end of namespace tavern::resource */

#endif /* end of define guard MATERIAL_MANAGER_H */
