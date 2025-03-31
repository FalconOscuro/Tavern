#ifndef PHYSICAL_MOUNT_H
#define PHYSICAL_MOUNT_H

#include "imount.h"

namespace tavern::file {

class physical_mount final : public imount
{
public:
    explicit physical_mount(const mount_path& mount_info);

    ~physical_mount() = default;

    bool has_file(const std::string_view path) const override;
    std::unique_ptr<ifile> load_file(const std::string_view path) const override;

    bool valid() const override;

    const std::string_view get_identifier() const override;

private:

    const std::string m_identifier;
}; /* end of class physical_mount : public imount */

} /* end of namespace tavern::file */

#endif /* end of define guard PHYSICAL_MOUNT_H */
