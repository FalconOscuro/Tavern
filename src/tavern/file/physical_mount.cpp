#include "tavern/file/physical_mount.h"

#include <filesystem>

#include "tavern/file/physical_file.h"

namespace tavern {

bool physical_mount::has_file(const std::string& path) const
{
    std::filesystem::path fpath(get_path() + path);

    return valid() && std::filesystem::exists(fpath)
        && !std::filesystem::is_directory(fpath);
}

std::unique_ptr<file> physical_mount::load_file(const std::string& path)
{
    if (!has_file(path))
        return nullptr;

    const std::string full_path = get_path() + path;
    return std::make_unique<physical_file>(full_path);
}

bool physical_mount::valid() const {
    return std::filesystem::is_directory(get_path());
}

} /* namespace tavern */
