#include "tavern/resource/mesh_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include <tiny_obj_loader.h>

#include <boost/log/trivial.hpp>

#include "tavern/resource/util/file.hpp"

namespace tavern::resource {

graphics::render_mesh* mesh_manager::load_new(const std::string& path)
{
    tinyobj::ObjReader reader;

    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = utility::get_file_parent_dir(path);

    if (!reader.ParseFromFile(path, config)) {
        BOOST_LOG_TRIVIAL(error) << "Failed to read obj file: " << reader.Error();
        return nullptr;
    }

    if (!reader.Warning().empty())
        BOOST_LOG_TRIVIAL(warning) << "Warning whilst reading obj file: " << reader.Warning();

    auto& attrib = reader.GetAttrib();
    auto& materials = reader.GetMaterials();

    // WARNING: Potentially dangerous type conversion
    graphics::render_mesh* m = new graphics::render_mesh(attrib.vertices, attrib.texcoords);

    return m;
}

} /* end of namespace tavern::resource */
