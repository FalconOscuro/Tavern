#include "tavern/file/path.h"

namespace tavern::file {


boost::log::record_ostream& operator<<(boost::log::record_ostream& stream, const mount_path& mount) {
    return stream << mount.identifer << ':' << mount.path;
}

} /* end of namespace tavern::file */
