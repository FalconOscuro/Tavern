#include "file/physical_mount.h"

#include <filesystem>

#include "file/physical_dir.h"
#include "file/physical_file.h"

namespace tavern::file {

physical_mount::physical_mount(const mount_path& mount_info):
    imount(mount_info.get_path()), m_identifier(mount_info.get_identifier())
{}

bool physical_mount::has_file(const std::string_view path) const
{
    std::filesystem::path fpath(get_path() + "/" + std::string(path));

    return valid() && std::filesystem::exists(fpath)
        && std::filesystem::is_regular_file(fpath);
}

file_handle physical_mount::load_file(const std::string_view path) const
{
    if (!has_file(path))
        return nullptr;

    return file_handle(new physical_file(mount_path(get_identifier(), path), get_path()));
}

bool physical_mount::has_dir(const std::string_view path) const
{
    std::filesystem::path dpath(get_path() + "/" + std::string(path));

    return valid() && std::filesystem::exists(dpath)
        && std::filesystem::is_directory(dpath);
}

dir_handle physical_mount::load_dir(const std::string_view path) const
{
    if (!has_dir(path))
        return nullptr;

    return dir_handle(new physical_dir(mount_path(get_identifier(), path), this, get_path()));
}

bool physical_mount::valid() const {
    return std::filesystem::is_directory(get_path());
}

const std::string_view physical_mount::get_identifier() const {
    return m_identifier;
}

} /* namespace tavern::file */
