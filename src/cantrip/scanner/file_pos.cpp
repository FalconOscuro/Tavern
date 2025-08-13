#include "cantrip/scanner/file_pos.h"

namespace cantrip {

std::string file_pos::to_string() const {
    return std::to_string(row).append(", ").append(std::to_string(col)).append(": ").append(std::to_string(indent));
}

bool file_pos::operator==(const file_pos& rhs) const
{
    return col == rhs.col && row == rhs.row
        && (
            file_name.data() == rhs.file_name.data() // fast check assume same file if using same pointer
            || file_name == rhs.file_name
    );
}

bool file_pos::operator!=(const file_pos& rhs) const
{
    return col != rhs.col || row != rhs.row
        || (
            file_name.data() != rhs.file_name.data()
            && file_name != rhs.file_name
    );
}


} /* namespace cantrip */
