#ifndef VIRTUAL_MOUNT_H
#define VIRTUAL_MOUNT_H

#include <unordered_map>

#include "imount.h"

namespace tavern::file {

class virtual_mount final : public imount
{
public:
    virtual_mount():
        imount("internal")
    {}

    ~virtual_mount() = default;

    bool has_file(const std::string& path) const override;
    [[nodiscard]] std::unique_ptr<ifile> load_file(const std::string& path) const override;

    bool valid() const override;

    void add_file(const std::string& path, const char* const data);

    const std::string_view get_identifier() const override;

private:

    std::unordered_map<std::string, const char* const> m_files;

}; /* end of class virtual_mount final : public imount */

} /* end of namespace tavern::file */

#endif /* end of define guard VIRTUAL_MOUNT_H */
