#include "tavern/file/tpk_mount.h"

#include <cstring>

#include <boost/log/trivial.hpp>

#include "tavern/file/tpk_file.h"
#include "tavern/file/physical_file.h"

namespace tavern::file {

tpk_mount::tpk_mount(const std::string& path):
    imount(path)
{
    physical_file file(path);

    if(!file.open())
        return;

    size_t bytes_read = file.get_str(reinterpret_cast<char*>(&m_header), sizeof(tpk_header));

    if (bytes_read != sizeof(tpk_header) || !valid())
        return;

    for (uint32_t i = 0; i < m_header.num_entries; ++i)
    {
        tpk_file_info f_info;

        bytes_read = file.get_str(reinterpret_cast<char*>(&f_info), sizeof(tpk_file_info));

        // invalidate signature to ensure valid returns false
        // malformed data
        if (bytes_read != sizeof(tpk_file_info)) {
            m_header.sig[0] = '\0';
            break;
        }

        m_file_table.emplace(f_info.path, f_info);
    }
}

bool tpk_mount::has_file(const std::string& path) const {
    return m_file_table.count(path);
}

std::unique_ptr<ifile> tpk_mount::load_file(const std::string& path) const
{
    if (!valid() || !has_file(path))
        return nullptr;

    auto f = m_file_table.find(path);

    return std::make_unique<tpk_file>(get_path(), f->second);
}

bool tpk_mount::valid() const {
    return m_header.fmt_version == TPK_VERSION && strcmp(m_header.sig, TPK_SIG);
}

} /* namespace tavern::file */
