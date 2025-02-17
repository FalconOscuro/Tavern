#include "tavern/file/tpk_mount.h"

#include <cstring>

#include "tavern/file/tpk_file.h"

namespace tavern {

tpk_mount::tpk_mount(const std::string& path):
    mount(path), m_file(path)
{
    if(!m_file.open())
        return;

    m_file.get_str(reinterpret_cast<char*>(&m_header), sizeof(tpk_header));

    if (!valid()) {
        m_file.close();
        return;
    }

    for (uint32_t i = 0; i < m_header.num_entries; ++i)
    {
        tpk_file_info f_info;

        m_file.get_str(reinterpret_cast<char*>(&f_info), sizeof(tpk_file_info));

        m_file_table.emplace(f_info.path, f_info);
    }

    m_file.close();
}

bool tpk_mount::has_file(const std::string& path) const {
    return m_file_table.count(path);
}

std::unique_ptr<file> tpk_mount::load_file(const std::string& path)
{
    if (!valid())
        return nullptr;

    return nullptr;
}

bool tpk_mount::valid() const {
    return m_header.fmt_version == TPK_VERSION && strcmp(m_header.sig, TPK_SIG);
}

} /* namespace tavern */
