#include "file/tpk_dir.h"

#include <cassert>

namespace tavern::file {

tpk_dir::tpk_dir(const mount_path& dir_path, const imount* mount, const dir_map* entries): idir(dir_path, mount)
{
    assert(entries != nullptr);

    for (const auto& dir_entry : *entries)
    {
        switch (dir_entry.second->type)
        {
        case tpk::FILE:
            m_files.emplace_back(dir_entry.first);
            break;

        case tpk::DIRECTORY:
            m_dirs.emplace_back(dir_entry.first);
            break;

        default:
            break;
        }
    }
}

size_t tpk_dir::num_files() const {
    return m_files.size();
}

size_t tpk_dir::num_dirs() const {
    return m_dirs.size();
}

mount_path tpk_dir::get_filename(size_t i) const {
    return mount_path(get_path().get_identifier(), std::string(get_path().get_path()) + '/' + std::string(m_files[i]));
}

mount_path tpk_dir::get_dirname(size_t i) const {
    return mount_path(get_path().get_identifier(), std::string(get_path().get_path()) + '/' + std::string(m_dirs[i]));
}

} /* namespace tavern::file */
