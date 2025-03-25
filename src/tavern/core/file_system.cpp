#include "tavern/core/file_system.h"

#include <boost/log/trivial.hpp>

#include "tavern/file/physical_mount.h"
#include "tavern/file/tpk_mount.h"

namespace tavern {

file_system::~file_system() {
    unmount_all();
}

const file::imount* file_system::mount_tpk(const std::string& path, std::string& identifier)
{
    // Slow if already loaded
    std::unique_ptr<file::tpk_mount> mount = std::make_unique<file::tpk_mount>(path);

    if (!mount->valid()) {
        BOOST_LOG_TRIVIAL(error) << "Failed whilst mounting tpk file(" << path << "), Invalid data";

        return nullptr;
    }

    identifier.resize(sizeof(file::tpk_header::name));
    strncpy(identifier.data(), mount->header().name, sizeof(file::tpk_header::name));

    // file already loaded or collision
    if (m_mounts.count(identifier))
    {
        const file::imount* loaded = m_mounts[identifier].get();

        if (loaded->get_path() == path) {
            BOOST_LOG_TRIVIAL(warning) << "TPK file(" << identifier << ':' << path << ") was already mounted!"
                " Should only be mounted once to avoid potential slowdown.";

            return loaded;
        }

        else {
            BOOST_LOG_TRIVIAL(error) << "Failed to mount TPK file(" << identifier << ':' << path << "), "
                "identifier already in use by " << identifier << ':' << path;
            return nullptr;
        }
    }

    BOOST_LOG_TRIVIAL(trace) << "Successfully mounted TPK file " << identifier << ':' << path;

    file::imount* ptr = mount.release();
    m_mounts.emplace(std::make_pair(identifier, std::unique_ptr<file::imount>(ptr)));
    return ptr;
}

const file::imount* file_system::mount_dir(const file::mount_path& mount_info)
{
    // file already loaded or collision
    if (m_mounts.count(mount_info.identifer))
    {
         const file::imount* loaded = m_mounts[mount_info.identifer].get();

        if (loaded->get_path() == mount_info.path) {
            BOOST_LOG_TRIVIAL(warning) << "Directory " << mount_info << " already mounted.";
            return loaded;
        }

        else {
            BOOST_LOG_TRIVIAL(error) << "Failed to mount Directory " << mount_info << ", identifier already in use by " << mount_info.identifer << ':' << loaded->get_path();
            return nullptr;
        }
    }

    file::physical_mount* mount = new file::physical_mount(mount_info.path);

    if (!mount->valid()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to mount Directory " << mount_info << ", was invalid";
        delete mount;
        return nullptr;
    }

    BOOST_LOG_TRIVIAL(trace) << "Successfully mounted Directory " << mount_info;

    m_mounts.emplace(std::make_pair(mount_info.identifer, std::unique_ptr<file::imount>(mount)));
    return mount;
}

void file_system::unmount_all() {
    for (auto it = m_mounts.cbegin(); it != m_mounts.cend(); it = unmount(it));
}

void file_system::unmount(const std::string& identifier)
{
    auto m = m_mounts.find(identifier);

    if (m == m_mounts.end())
        return;

    m_mounts.erase(m);
}

file_system::mount_map_type::const_iterator file_system::unmount(mount_map_type::const_iterator it)
{
    BOOST_LOG_TRIVIAL(trace) << "Unmounted " << it->first << ':' << it->second->get_path();
    return m_mounts.erase(it);
}

} /* end of namespace tavern */
