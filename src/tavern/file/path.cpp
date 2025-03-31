#include "tavern/file/path.h"

#include <algorithm>
#include <cassert>

#include <ryml_std.hpp>
#include <c4/yml/node.hpp>

namespace tavern::file {

mount_path::mount_path(const std::string_view identifier, const std::string_view path) {
    set_full_path(identifier, path);
}

const std::string_view mount_path::set_identifier(const std::string_view identifier) {
    const std::string path = std::string(get_path());

    return set_full_path(identifier, path);
}

const std::string_view mount_path::get_identifier() const {
    return std::string_view(m_full_path.c_str(), get_split_pos());
}

const std::string_view mount_path::set_path(const std::string_view path) {
    const size_t path_start_pos = get_split_pos() + 1;

    m_full_path.erase(path_start_pos + 1);
    m_full_path.reserve(m_full_path.length() + path.length());
    m_full_path.append(path);

    return std::string_view(m_full_path.c_str() + path_start_pos, path.length());
}

const std::string_view mount_path::get_path() const {
    const size_t path_start_pos = std::min(get_split_pos() + 1, m_full_path.length());
    return std::string_view(m_full_path.c_str() + path_start_pos, m_full_path.length() - path_start_pos);
}

bool mount_path::operator==(const mount_path& mp) const {
    return m_full_path == mp.m_full_path;
}

bool mount_path::operator!=(const mount_path& mp) const {
    return m_full_path != mp.m_full_path;
}

const std::string_view mount_path::set_full_path(const std::string_view identifier, const std::string_view path)
{
    m_full_path.clear();
    assert(identifier.find(':') == identifier.npos && "Identifier should not contain reserved char ':'");

    m_full_path.reserve(identifier.size() + path.size() + 1);

    m_full_path.append(identifier);
    m_full_path.push_back(':');
    m_full_path.append(path);

    return std::string_view(m_full_path.c_str(), identifier.length());
}

size_t mount_path::get_split_pos() const {
    size_t split = m_full_path.find_first_of(':');

    assert(split != m_full_path.npos);

    return split == m_full_path.npos ? 0 : split;
}

std::ostream& operator<<(std::ostream& stream, const mount_path& mount) {
    return stream << '[' << mount.get_identifier() << "]:" << mount.get_path();
}

bool read(const ryml::ConstNodeRef& n, mount_path* val)
{
    std::string identifier, path;

    n["identifier"] >> identifier;
    n["path"] >> path;

    *val = mount_path(identifier, path);

    return true;
}

void write(ryml::NodeRef* n, const mount_path& val)
{
    *n |= ryml::MAP;
    n->append_child() << ryml::key("identifier") << std::string(val.get_identifier());
    n->append_child() << ryml::key("path") << std::string(val.get_path());
}

} /* end of namespace tavern::file */
