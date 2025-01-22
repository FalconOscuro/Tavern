#ifndef TEXTURE2D_MANAGER_H
#define TEXTURE2D_MANAGER_H

#include "resource_type_manager.hpp"
#include "../graphics/graphics.h"

namespace tavern::resource {

class texture2d_manager : public resource_type_manager<graphics::texture2d>
{

protected:
    graphics::texture2d* load_new(const std::string& path) override;

}; /* end of class texture2d_manager : public resource_type_manager<graphics::texture2d> */

} /* end of namespace tavern::resource */

#endif /* end of define guard TEXTURE2D_MANAGER_H */
