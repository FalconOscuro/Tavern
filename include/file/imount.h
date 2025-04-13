#ifndef IMOUNT_H
#define IMOUNT_H

#include <string>

#include "ifile.h"
#include "path.h"

namespace tavern::file {

class imount
{
public:
    imount(const std::string_view path):
        m_path(path)
    {}

    virtual ~imount() = default;

    virtual bool has_file(const std::string_view path) const = 0;
    [[nodiscard]] virtual file_handle load_file(const std::string_view path) const = 0;

    virtual bool valid() const = 0;

    const std::string& get_path() const {
        return m_path;
    }

    virtual const std::string_view get_identifier() const = 0;

    mount_path get_mount_info() const {
        return mount_path(get_identifier(), m_path);
    }

    // list files?
    // list subdirs?

private:

    const std::string m_path;

}; /* end of class imount */

} /* end of namespace tavern::file */

#endif /* end of define guard IMOUNT_H */
