#ifndef TPK_DIR_H
#define TPK_DIR_H

#include "idir.h"

#include <vector>

#include "tpk_mount.h"

namespace tavern::file {

class tpk_dir final : public idir
{
    using dir_map = file_tree_node::dir_map;

public:

    tpk_dir(const mount_path& dir_path, const imount* mount, const dir_map* entries);
    ~tpk_dir() = default;

    size_t num_files() const override;
    size_t num_dirs() const override;

    // need to strip identifer
    mount_path get_filename(size_t i) const override;
    mount_path get_dirname(size_t i) const override;

private:

    // both implementations of idir use vectors, could change to be function of base class & use iterators?
    std::vector<std::string_view> m_files;
    std::vector<std::string_view> m_dirs;
};

} /* namespace tavern::file */

#endif /* end of include guard: TPK_DIR_H */
