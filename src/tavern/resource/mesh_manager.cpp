#include "tavern/resource/mesh_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include <tiny_obj_loader.h>

#include <boost/log/trivial.hpp>

namespace tavern::resource {

graphics::mesh* mesh_manager::load_new(const std::string& path)
{
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path)) {
        BOOST_LOG_TRIVIAL(error) << "Failed to read obj file: " << reader.Error();
        return nullptr;
    }

    if (!reader.Warning().empty())
        BOOST_LOG_TRIVIAL(warning) << "Warning whilst reading obj file: " << reader.Warning();

    auto& attrib = reader.GetAttrib();

    // WARNING: Potentially dangerous type conversion
    graphics::mesh* m = new graphics::mesh(reinterpret_cast<const maths::vector3f*>(attrib.vertices.data()), attrib.vertices.size());

    return m;
}

} /* end of namespace tavern::resource */
