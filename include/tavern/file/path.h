#ifndef PATH_H
#define PATH_H

#include <string>
#include <ostream>

namespace tavern::file {

struct mount_path
{
    mount_path() = default;
    mount_path(const std::string_view identifer, const std::string_view path);

    ~mount_path() = default;

    std::string identifer;

    std::string path;
}; /* end of struct mount_path */

std::ostream& operator<<(std::ostream& stream, const mount_path& mount);

} /* end of namespace tavern::file */

#endif /* end of define guard PATH_H */
