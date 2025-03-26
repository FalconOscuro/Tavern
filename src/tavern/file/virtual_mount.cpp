#include "tavern/file/virtual_mount.h"

#include "tavern/file/virtual_file.h"

namespace tavern::file {

bool virtual_mount::has_file(const std::string& path) const {
    return m_files.count(path);
}

std::unique_ptr<ifile> virtual_mount::load_file(const std::string& path) const
{
    auto f = m_files.find(path);

    if (f == m_files.end())
        return nullptr;

    return std::make_unique<virtual_file>(f->second, path);
}

bool virtual_mount::valid() const {
    return true;
}

const std::string_view virtual_mount::get_identifier() const {
    return "internal";
}

} /* end of namespace tavern::file */
