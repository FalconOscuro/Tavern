#ifndef IMOUNT_H
#define IMOUNT_H

#include <memory>
#include <string>

#include "ifile.h"
#include "path.h"

namespace tavern::file {

class imount
{
public:
    imount(const std::string& path):
        m_path(path)
    {}

    virtual ~imount() = default;

    virtual bool has_file(const std::string& path) const = 0;
    [[nodiscard]] virtual std::unique_ptr<ifile> load_file(const std::string& path) const = 0;

    virtual bool valid() const = 0;

    const std::string& get_path() const {
        return m_path;
    }

private:

    const std::string m_path;
}; /* end of class imount */

} /* end of namespace tavern::file */

#endif /* end of define guard IMOUNT_H */
