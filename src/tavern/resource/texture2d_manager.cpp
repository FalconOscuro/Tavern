#include "tavern/resource/texture2d_manager.h"

#include <boost/log/trivial.hpp>

#include <stb_image.h>

namespace tavern::resource {

int file_read_callback(void* file, char* data, int size) {
    return reinterpret_cast<file::ifile*>(file)->get_str(data, size);
}

void file_skip_callback(void* file, int n) {
    reinterpret_cast<file::ifile*>(file)->seek(n, file::CURRENT);
}

int file_eof_callback(void* file) {
    return reinterpret_cast<file::ifile*>(file)->eof();
}

graphics::texture2d* texture2d_manager::load_new(file::ifile* file)
{
    glm::ivec3 size;

    stbi_io_callbacks clbk;
    clbk.read = &file_read_callback;
    clbk.skip = &file_skip_callback;
    clbk.eof  = &file_eof_callback;

    auto data = std::unique_ptr<unsigned char, decltype(stbi_image_free)*>(
        stbi_load_from_callbacks(&clbk, file, &size.x, &size.y, &size.z, 0),
        stbi_image_free
    );

    if (!data) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load texture: " << file->get_path();
        return nullptr;
    }

    graphics::texture2d* tex = new graphics::texture2d(data.get(), size);
    return tex;
}

} /* end of namespace tavern::resource */
