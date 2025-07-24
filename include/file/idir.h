#ifndef IDIR_H
#define IDIR_H

#include "path.h"
#include "ifile.h"

namespace tavern::file {

class idir;
class imount;

typedef std::unique_ptr<idir> dir_handle;

class idir
{
public:

    virtual size_t num_dirs() const = 0;
    virtual size_t num_files() const = 0;

    // path?
    virtual mount_path get_filename(size_t i) const = 0;
    virtual mount_path get_dirname(size_t i) const = 0;

    file_handle get_file(size_t i) const;
    dir_handle get_dir(size_t i) const;

    const mount_path& get_path() const {
        return m_path;
    }

    const imount* get_mount() const {
        return m_mount;
    }

protected:

    explicit idir(const mount_path& path, const imount* mount):
        m_path(path), m_mount(mount)
    {}

private:

    const mount_path m_path;
    const imount* m_mount;
};

} /* namespace tavern::file */

#endif /* end of include guard: IDIR_H */
