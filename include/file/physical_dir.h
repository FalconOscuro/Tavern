#ifndef PHYSICAL_DIR_H
#define PHYSICAL_DIR_H

#include "idir.h"

#include <vector>

namespace tavern::file {

class physical_dir final : public idir
{
public:

    explicit physical_dir(const mount_path& dir_path, const imount* mount, const std::string_view mount_path);

    // allow unmounted dirs?
    
    ~physical_dir() = default;

    size_t num_files() const override;
    size_t num_dirs() const override;

    mount_path get_filename(size_t i) const override;
    mount_path get_dirname(size_t i) const override;

private:

    std::vector<std::string> m_files;
    std::vector<std::string> m_dirs;
};

} /* namespace tavern::file */

#endif /* end of include guard: PHYSICAL_DIR_H */
