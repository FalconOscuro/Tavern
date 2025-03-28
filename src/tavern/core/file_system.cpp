#include "tavern/core/file_system.h"

#include <filesystem>

#include <boost/log/trivial.hpp>

#include "tavern/file/physical_mount.h"
#include "tavern/file/tpk_mount.h"
#include "tavern/file/virtual_mount.h"

namespace tavern {

file_system::~file_system() {
    unmount_all();
}

bool file_system::init() {
    m_mounts.emplace("internal", std::make_unique<file::virtual_mount>());

    return true;
}

const file::imount* file_system::mount_tpk(std::string path, std::string& identifier)
{
    path = make_path_relative(path);
    if (path.empty())
        return nullptr;

    // Slow if already loaded
    std::unique_ptr<file::tpk_mount> mount = std::make_unique<file::tpk_mount>(path);

    if (!mount->valid()) {
        BOOST_LOG_TRIVIAL(error) << "Failed whilst mounting tpk file '" << path << "', Invalid data";

        return nullptr;
    }

    identifier = mount->get_identifier();

    if (identifier.empty()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to mount tpk file '" << path << "', name was null";
        return nullptr;
    }

    // file already loaded or collision
    if (is_mounted(identifier))
    {
        const file::imount* loaded = m_mounts[identifier].get();

        if (loaded->get_path() == path) {
            BOOST_LOG_TRIVIAL(warning) << "TPK file '" << identifier << ':' << path << "' was already mounted!"
                " Should only be mounted once to avoid potential slowdown.";

            return loaded;
        }

        else {
            BOOST_LOG_TRIVIAL(error) << "Failed to mount TPK file '" << identifier << ':' << path
                << "', identifier already in use by '" << loaded->get_mount_info() << '\'';
            return nullptr;
        }
    }

    BOOST_LOG_TRIVIAL(trace) << "Successfully mounted TPK file '" << mount->get_mount_info() << '\'';

    file::imount* ptr = mount.release();
    m_mounts.emplace(std::make_pair(identifier, std::unique_ptr<file::imount>(ptr)));
    return ptr;
}

const file::imount* file_system::mount_dir(file::mount_path mount_info)
{
    mount_info.path = make_path_relative(mount_info.path);

    if (mount_info.identifer.empty() || mount_info.path.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to mount Directory '" << mount_info << "', identifier/path cannot be null";
        return nullptr;
    }

    // file already loaded or collision
    if (is_mounted(mount_info.identifer))
    {
         const file::imount* loaded = m_mounts[mount_info.identifer].get();

        if (loaded->get_path() == mount_info.path) {
            BOOST_LOG_TRIVIAL(warning) << "Directory '" << mount_info << "' already mounted.";
            return loaded;
        }

        else {
            BOOST_LOG_TRIVIAL(error) << "Failed to mount Directory '" << mount_info << "', identifier already in use by '" << mount_info.identifer << ':' << loaded->get_path() << '\'';
            return nullptr;
        }
    }

    file::physical_mount* mount = new file::physical_mount(mount_info);

    if (!mount->valid()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to mount Directory '" << mount_info << "', was invalid";
        delete mount;
        return nullptr;
    }

    BOOST_LOG_TRIVIAL(trace) << "Successfully mounted Directory '" << mount_info << '\'';

    m_mounts.emplace(std::make_pair(mount_info.identifer, std::unique_ptr<file::imount>(mount)));
    return mount;
}

bool file_system::file_exists(const file::mount_path& file_path) const
{
    auto mount = find(file_path.path);

    return mount != end() && mount->second->has_file(file_path.path);
}

std::unique_ptr<file::ifile> file_system::load_file(const file::mount_path& file_path) const
{
    auto mount = find(file_path.identifer);

    if (mount == end())
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to load File '" << file_path << "', unknown identifier";

        return nullptr;
    }

    auto file = mount->second->load_file(file_path.path);

    if (!file)
        BOOST_LOG_TRIVIAL(error) << "Failed to load File '" << file_path << "', could not resolve path";

    else
        BOOST_LOG_TRIVIAL(trace) << "Successfully loaded File '" << file_path << '\'';

    return file;
}

bool file_system::is_mounted(const std::string& identifier) const {
    return m_mounts.count(identifier);
}

void file_system::unmount_all() {
    for (auto it = begin(); it != end(); it = unmount(it));
}

void file_system::unmount(const std::string& identifier)
{
    auto m = find(identifier);

    if (m == end())
        return;

    unmount(m);
}

file_system::mount_map_type::const_iterator file_system::unmount(mount_map_type::const_iterator it)
{
    if (it == end())
        return it;

    BOOST_LOG_TRIVIAL(trace) << "Unmounted '" << it->second->get_mount_info() << '\'';
    return m_mounts.erase(it);
}

std::string file_system::make_path_relative(const std::string& path)
{
    const auto base = std::filesystem::current_path();
    const auto path_absolute = std::filesystem::absolute(path);

    if (base == path_absolute)
        BOOST_LOG_TRIVIAL(warning) << "Tried to find relative path to '" << path_absolute
            << "', but was the same as base!";

    std::error_code ec;

    const auto path_relative = std::filesystem::relative(path_absolute, base, ec);

    if (ec) {
        BOOST_LOG_TRIVIAL(error) << "Could not resolve relative path to '" << path_absolute
            << "' from '" << base << "', Error: " << ec.message();
        return "";
    }

    return path_relative;
}

} /* end of namespace tavern */
