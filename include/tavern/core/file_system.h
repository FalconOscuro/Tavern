#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <memory>
#include <string>
#include <unordered_map>

#include "tavern/file/imount.h"

namespace tavern {

class file_system
{
public:

    ~file_system();

    file_system(const file_system&) = delete;
    void operator=(const file_system&) = delete;

    const file::imount* mount_tpk(const std::string& path, std::string& identifier);
    const file::imount* mount_dir(const file::mount_path& mount_info);

    void unmount_all();
    void unmount(const std::string& identifier);

    [[nodiscard]] static file_system& singleton() {
        static file_system instance;
        return instance;
    }

private:
    using mount_map_type = std::unordered_map<std::string, std::unique_ptr<file::imount>>;

    file_system() = default;

    mount_map_type::const_iterator unmount(mount_map_type::const_iterator it);

    mount_map_type m_mounts;
}; /* end of class file_system */

} /* end of namespace tavern */

#endif /* end of define guard FILE_SYSTEM_H */
