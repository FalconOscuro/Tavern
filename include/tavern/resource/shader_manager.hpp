#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include "resource_type_manager.hpp"
#include "tavern/graphics/shader.h"

namespace tavern::resource {

class shader_manager : public resource_type_manager<graphics::shader>
{
protected:

    resource_type* load_new(file::ifile* file) override;

}; /* end of class shader_manager : public resource_type_manager<graphics::shader> */

} /* end of namespace tavern::resource */

#endif /* end of define guard SHADER_MANAGER_HPP */
