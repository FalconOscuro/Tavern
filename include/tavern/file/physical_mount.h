#ifndef PHYSICAL_MOUNT_H
#define PHYSICAL_MOUNT_H

#include "imount.h"

namespace tavern::file {

class physical_mount : public imount
{
public:
    explicit physical_mount(const std::string& path):
        imount(path)
    {}

    ~physical_mount() = default;

    bool has_file(const std::string& path) const override;
    std::unique_ptr<ifile> load_file(const std::string& path) const override;

    bool valid() const override;
}; /* end of class physical_mount : public imount */

} /* end of namespace tavern::file */

#endif /* end of define guard PHYSICAL_MOUNT_H */
