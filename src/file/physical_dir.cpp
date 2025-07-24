#include "file/physical_dir.h"

#include <cassert>
#include <filesystem>

namespace tavern::file {

physical_dir::physical_dir(const mount_path& dir_path, const imount* mount, const std::string_view mount_path): idir(dir_path, mount)
{
    const std::filesystem::path dir = 
        std::filesystem::path(std::string(mount_path)
        + "/" + std::string(dir_path.get_path()));

    // should be caught by mount before reaching here
    assert(std::filesystem::exists(dir) && std::filesystem::is_directory(dir));

    // find contents of dir
    for (const auto& dir_entry : std::filesystem::directory_iterator(dir, std::filesystem::directory_options::skip_permission_denied))
    {
        if (dir_entry.is_regular_file())
            m_files.emplace_back(dir_entry.path().filename());

        else if (dir_entry.is_directory())
            m_dirs.emplace_back(dir_entry.path().filename());
    }
}

size_t physical_dir::num_files() const {
    return m_files.size();
}

size_t physical_dir::num_dirs() const {
    return m_dirs.size();
}

mount_path physical_dir::get_filename(size_t i) const {
    return mount_path(get_path().get_identifier(), std::string(get_path().get_path()) + '/' + m_files[i]);
}

mount_path physical_dir::get_dirname(size_t i) const {
    return mount_path(get_path().get_identifier(), std::string(get_path().get_path()) + '/' + m_dirs[i]);
}

} /* namespace tavern::file */
