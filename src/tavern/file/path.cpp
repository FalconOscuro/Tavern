#include "tavern/file/path.h"

namespace tavern::file {

mount_path::mount_path(const std::string_view identifer, const std::string_view path):
    identifer(identifer), path(path)
{}

std::ostream& operator<<(std::ostream& stream, const mount_path& mount) {
    return stream << '[' << mount.identifer << "]:" << mount.path;
}

} /* end of namespace tavern::file */
