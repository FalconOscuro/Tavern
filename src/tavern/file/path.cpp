#include "tavern/file/path.h"
#include "c4/yml/node.hpp"

#include <ryml_std.hpp>

namespace tavern::file {

mount_path::mount_path(const std::string_view identifer, const std::string_view path):
    identifer(identifer), path(path)
{}

std::ostream& operator<<(std::ostream& stream, const mount_path& mount) {
    return stream << '[' << mount.identifer << "]:" << mount.path;
}

bool read(const ryml::ConstNodeRef& n, mount_path* val)
{
    n["identifier"] >> val->identifer;
    n["path"] >> val->path;

    return true;
}

void write(ryml::NodeRef* n, const mount_path& val)
{
    *n |= ryml::MAP;
    n->append_child() << ryml::key("identifier") << val.identifer;
    n->append_child() << ryml::key("path") << val.path;
}

} /* end of namespace tavern::file */
