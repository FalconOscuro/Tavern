#include "tavern/resource/texture2d_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <boost/log/trivial.hpp>

namespace tavern::resource {

graphics::texture2d* texture2d_manager::load_new(const std::string& path)
{
    maths::vector3i size;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &size.x, &size.y, &size.z, 0);

    if (!data) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load texture: " << path;
        return nullptr;
    }

    graphics::texture2d* tex = new graphics::texture2d(data, size);
    stbi_image_free(data);
    return tex;
}

} /* end of namespace tavern::resource */
