#include "tavern/resource/texture2d_manager.h"

#include <boost/log/trivial.hpp>

namespace tavern::resource {

graphics::texture2d* texture2d_manager::load_new(const std::string& path)
{
    glm::ivec3 size;
    auto data = graphics::load_image_data(path.c_str(), size);

    if (!data) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load texture: " << path;
        return nullptr;
    }

    graphics::texture2d* tex = new graphics::texture2d(data.get(), size);
    return tex;
}

} /* end of namespace tavern::resource */
