#include "tavern/file/path.h"

namespace tavern::file {

std::ostream& operator<<(std::ostream& stream, const mount_path& mount) {
    return stream << mount.identifer << ':' << mount.path;
}

} /* end of namespace tavern::file */
