#include "file/idir.h"

#include "file/imount.h"

namespace tavern::file {

file_handle idir::get_file(size_t i) const
{
    if (i >= num_files())
        return nullptr;

    return m_mount->load_file(get_filename(i).get_path());
}

dir_handle idir::get_dir(size_t i) const
{
    if (i >= num_dirs())
        return nullptr;

    return m_mount->load_dir(get_dirname(i).get_path());
}

} /* namespace tavern::file */
