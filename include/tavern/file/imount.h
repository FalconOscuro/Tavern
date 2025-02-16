#ifndef IMOUNT_H
#define IMOUNT_H

#include <memory>
#include <string>

#include "ifile.h"

namespace tavern {

class mount
{
public:
    mount(const std::string& path):
        m_path(path)
    {}

    virtual ~mount() = default;

    virtual bool has_file(const std::string& path) const = 0;
    virtual std::unique_ptr<file> load_file(const std::string& path) = 0;

    const std::string& get_path() const {
        return m_path;
    }

private:

    const std::string m_path;
}; /* end of class mount */

} /* end of namespace tavern */

#endif /* end of define guard IMOUNT_H */
