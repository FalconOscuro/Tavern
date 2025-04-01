#include "tavern/resource/util/assimp_io.h"

#include <boost/log/trivial.hpp>

#include "tavern/core/file_system.h"

namespace tavern::resource::util {

assimp_iostream::assimp_iostream(file::ifile* file): m_file(file)
{}

size_t assimp_iostream::Read(void* buf, size_t size, size_t count) {
    return m_file->get_str(reinterpret_cast<char*>(buf), size * count);
}

size_t assimp_iostream::Write(const void* buf, size_t size, size_t count) {
    (void) buf;
    (void) size;
    (void) count;

    BOOST_LOG_TRIVIAL(error) << "Custom assimp_iostream does not support file write functionality!";
    return 0;
}

aiReturn assimp_iostream::Seek(size_t offset, aiOrigin origin)
{
    long l_offset = offset;

    file::origin mode;
    switch (origin)
    {
    case aiOrigin_SET:
        mode = file::START;
        break;

    case aiOrigin_END:
        mode = file::END;
        l_offset *= -1;
        break;

    default:
        mode = file::CURRENT;
        break;
    }

    return m_file->seek(l_offset, mode) ? aiReturn::aiReturn_SUCCESS : aiReturn::aiReturn_FAILURE;
}

size_t assimp_iostream::Tell() const {
    return m_file->pos();
}

size_t assimp_iostream::FileSize() const {
    return m_file->size();
}

void assimp_iostream::Flush()
{}

bool assimp_iosystem::Exists(const char* file) const
{
    file::mount_path path;
    // invalid path
    if (!file::mount_path::try_create(file, path))
        return false;

    return file_system::singleton().file_exists(path);
}

Assimp::IOStream* assimp_iosystem::Open(const char* file, const char* mode)
{
    file::mount_path path;
    //invalid path
    if (!file::mount_path::try_create(file, path))
        return nullptr;

    if (mode[0] == 'w') {
        BOOST_LOG_TRIVIAL(error) << "Custom assimp_iosystem does not support file writing!";
        return nullptr;
    }

    auto f = file_system::singleton().load_file(path);
    if (!(f && f->open()))
        return nullptr;

    return new assimp_iostream(f.release());
}

void assimp_iosystem::Close(Assimp::IOStream* file) {
    delete file;
}

bool assimp_iosystem::CreateDirectory(const std::string& path) {
    BOOST_LOG_TRIVIAL(error) << "Custom assimp_iosystem cannot create directory '" << path << "', unsupported";
    return false;
}

bool assimp_iosystem::DeleteFile(const std::string& path) {
    BOOST_LOG_TRIVIAL(error) << "Custom assimp_iosystem cannot delete file '" << path << "', unsupported";
    return false;
}

} /* end of namespace tavern::resource::util */
