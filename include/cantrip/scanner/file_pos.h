#ifndef FILE_POS_H
#define FILE_POS_H

#include <cstdint>
#include <string_view>

namespace cantrip {

struct file_pos
{
    std::string_view file_name;

    uint32_t col = 0;
    uint32_t row = 0;

    uint32_t indent = 0;

    // 4 bytes padding un-used

    // NOTE: Equality operators do not account for indent, they only compare the absolute file position
    bool operator==(const file_pos& rhs) const;
    bool operator!=(const file_pos& rhs) const;
    // cleanup placement for operator definitions?
    // currently in file_interface.cpp w/ only indirect link back here
}; /* end of struct file_pos */

} /* namespace cantrip */

#endif /* end of include guard: FILE_POS_H */
