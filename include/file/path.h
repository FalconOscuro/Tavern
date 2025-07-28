#ifndef PATH_H
#define PATH_H

#include <functional>
#include <string>
#include <ostream>

namespace tavern::file {

class mount_path
{
public:
    mount_path() = default;
    mount_path(const mount_path&) = default;
    mount_path(const std::string_view identifer, const std::string_view path);

    ~mount_path() = default;

    const std::string_view set_identifier(const std::string_view identifier);
    const std::string_view get_identifier() const;

    const std::string_view set_path(const std::string_view path);
    const std::string_view get_path() const;

    const std::string_view get_full() const {
        return m_full_path;
    }

    const std::string_view get_extension() const;

    // get parent dir/endpoint name?

    bool operator==(const mount_path& mp) const;
    bool operator!=(const mount_path& mp) const;

    static bool try_create(const std::string_view s, mount_path& path);

private:

    const std::string_view set_full_path(const std::string_view identifier, const std::string_view path);

    size_t get_split_pos() const;

    std::string m_full_path;
    // cache split pos?
}; /* end of class mount_path */

std::ostream& operator<<(std::ostream& stream, const mount_path& mount);

} /* end of namespace tavern::file */

template <>
struct std::hash<tavern::file::mount_path>
{
    size_t operator()(const tavern::file::mount_path& path) const noexcept {
        return std::hash<std::string_view>{}(path.get_full());
    }
}; /* end of struct std::hash<mount_path> */

#endif /* end of define guard PATH_H */
