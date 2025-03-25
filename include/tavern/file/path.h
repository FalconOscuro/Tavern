#ifndef PATH_H
#define PATH_H

#include <ostream>
#include <string>

namespace tavern::file {

struct mount_path
{
    std::string identifer;

    std::string path;
}; /* end of struct mount_path */

std::ostream& operator<<(std::ostream& stream, const mount_path& mount);

} /* end of namespace tavern::file */

#endif /* end of define guard PATH_H */
