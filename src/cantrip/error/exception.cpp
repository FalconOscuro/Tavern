#include "cantrip/error/exception.h"

#include <cstring>
#include <string>

namespace cantrip::error {

exception::exception(const file_pos& pos, const char* err_msg):
    m_pos(pos)
{
    const std::string row = std::to_string(pos.row);
    const std::string col = std::to_string(pos.col);

    const size_t row_len = row.size();
    const size_t col_len = col.size();

    // file pos string representation:
    // "[$row, $col]: "
    // open sqr bracket len = 1
    // row_str len
    // comma separator len = 2
    // col_str len
    // close square bracket & separator len = 3
    const size_t pos_len = 1 + row_len + 2 + col_len + 3;
    const size_t err_msg_len = strlen(err_msg);

    // extra byte for nullchar
    m_err_str = new char[pos_len + err_msg_len + 1];

    // file_pos
    m_err_str[0] = '[';
    memcpy(m_err_str + 1, row.c_str(), row_len);
    memcpy(m_err_str + 1 + row_len, ", ", 2);
    memcpy(m_err_str + 1 + row_len + 2, col.c_str(), col_len);
    memcpy(m_err_str + 1 + row_len + 2 + col_len, "]: ", 3);

    // err_msg
    memcpy(m_err_str + pos_len, err_msg, err_msg_len);

    // null terminator
    m_err_str[pos_len + err_msg_len] = '\0';
}

exception::~exception() {
    delete[] m_err_str;
}

const char* exception::what() const noexcept {
    return m_err_str;
}

} /* namespace cantrip::error */
