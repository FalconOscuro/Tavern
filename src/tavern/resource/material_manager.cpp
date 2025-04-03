#include "tavern/resource/material_manager.h"

#include <boost/log/trivial.hpp>

#include "tavern/resource/util/obj_loader.h"

namespace tavern::resource {

graphics::material* material_manager::load_new(file::ifile* file)
{
    graphics::material material;
    if (!(util::load_mtl(file, material)))
         return nullptr;

    return new graphics::material(material);
}

} /* end of namespace tavern::resource */
