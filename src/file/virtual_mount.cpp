#include "file/virtual_mount.h"

#include "file/virtual_file.h"

namespace tavern::file {

bool virtual_mount::has_file(const std::string_view path) const {
    return m_files.count(std::string(path));
}

file_handle virtual_mount::load_file(const std::string_view path) const
{
    auto f = m_files.find(std::string(path));

    if (f == m_files.end())
        return nullptr;

    return file_handle(new virtual_file(f->second, path));
}

bool virtual_mount::has_dir(const std::string_view path) const
{
    // unsupported
    (void)path;
    return false;
}

dir_handle virtual_mount::load_dir(const std::string_view path) const
{
    // unsupported
    (void)path;
    return nullptr;
}

bool virtual_mount::valid() const {
    return true;
}

const std::string_view virtual_mount::get_identifier() const {
    return "internal";
}

} /* end of namespace tavern::file */
