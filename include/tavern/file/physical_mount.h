#ifndef PHYSICAL_MOUNT_H
#define PHYSICAL_MOUNT_H

#include "imount.h"

namespace tavern {

class physical_mount : public mount
{
public:
    explicit physical_mount(const std::string& path):
        mount(path)
    {}

    ~physical_mount() = default;

    bool has_file(const std::string& path) const override;
    std::unique_ptr<file> load_file(const std::string& path) override;
}; /* end of class physical_mount : public mount */

} /* end of namespace tavern */

#endif /* end of define guard PHYSICAL_MOUNT_H */
