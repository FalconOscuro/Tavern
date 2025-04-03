#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <vector>

#include "tavern/graphics/material.h"
#include "tavern/graphics/vertex.h"
#include "tavern/file/ifile.h"

namespace tavern::resource::util {

bool load_obj(file::ifile* file, std::vector<graphics::vertex>& vertices, std::vector<graphics::face>& faces);

bool load_mtl(file::ifile* file, tavern::graphics::material& material);

} /* end of namespace tavern::resource::util */

#endif /* end of define guard OBJ_LOADER_H */
