#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <memory>
#include <string>
#include <unordered_map>

#include "tavern/file/imount.h"

namespace tavern {

class file_system
{
    using mount_map_type = std::unordered_map<std::string, std::unique_ptr<file::imount>>;

public:

    using iterator = mount_map_type::const_iterator;

    ~file_system();

    file_system(const file_system&) = delete;
    void operator=(const file_system&) = delete;

    bool init();

    // do we need to return ptr? all could be managed through file_system singleton
    const file::imount* mount_tpk(std::string path, std::string& identifier);
    const file::imount* mount_dir(file::mount_path mount_info);
    const file::imount* mount(file::mount_path mount_info);

    bool file_exists(const file::mount_path& file_path) const;
    file::file_handle load_file(const file::mount_path& file_path) const;

    bool is_mounted(const std::string& identifier) const;

    void unmount_all();
    void unmount(const std::string& identifier);
    iterator unmount(iterator it);

    iterator find(const std::string& identifer) const {
        return m_mounts.find(identifer);
    }

    iterator begin() const {
        return m_mounts.cbegin();
    }

    iterator end() const {
        return m_mounts.end();
    }

    [[nodiscard]] static file_system& singleton() {
        static file_system instance;
        return instance;
    }

private:

    file_system() = default;

    // returns empty string if unable to resolve
    // NOTE: if path == base acts same as being unable to resolve
    static std::string make_path_relative(const std::string_view path);


    mount_map_type m_mounts;
}; /* end of class file_system */

} /* end of namespace tavern */

#endif /* end of define guard FILE_SYSTEM_H */
