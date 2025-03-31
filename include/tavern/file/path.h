#ifndef PATH_H
#define PATH_H

#include <string>
#include <ostream>

#include <ryml.hpp>

namespace tavern::file {

class mount_path
{
public:
    mount_path() = default;
    mount_path(const std::string_view identifer, const std::string_view path);

    ~mount_path() = default;

    const std::string_view set_identifier(const std::string_view identifier);
    const std::string_view get_identifier() const;

    const std::string_view set_path(const std::string_view path);
    const std::string_view get_path() const;

    const std::string_view get_full() const {
        return m_full_path;
    }

    bool operator==(const mount_path& mp) const;
    bool operator!=(const mount_path& mp) const;

private:

    const std::string_view set_full_path(const std::string_view identifier, const std::string_view path);

    size_t get_split_pos() const;

    std::string m_full_path;
}; /* end of class mount_path */

std::ostream& operator<<(std::ostream& stream, const mount_path& mount);

bool read(const ryml::ConstNodeRef& n, mount_path* val);
void write(ryml::NodeRef* n, const mount_path& val);

} /* end of namespace tavern::file */

#endif /* end of define guard PATH_H */
