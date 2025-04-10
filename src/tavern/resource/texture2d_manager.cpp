#include "tavern/resource/texture2d_manager.h"

#include <boost/log/trivial.hpp>

#include <stb_image.h>

namespace tavern::resource {

int file_read_callback(void* p, char* data, int size)
{
    file::ifile* file = reinterpret_cast<file::ifile*>(p);
    int bytes_read = file->get_str(data, size);

    //assert(bytes_read == size && "Read fewer bytes than anticipated");
    return bytes_read;
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

    unsigned char* data =
        stbi_load_from_callbacks(&clbk, file,
                                 &size.x, &size.y, &size.z,
                                 0
        );

    if (!data) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load texture " << file->get_path() << ": " << stbi_failure_reason();
        return nullptr;
    }

    graphics::texture2d* tex = new graphics::texture2d(data, size);
    stbi_image_free(data);
    return tex;
}

} /* end of namespace tavern::resource */
