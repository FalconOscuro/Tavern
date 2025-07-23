#include "cantrip/error/exception.h"

#include <cstring>
#include <string>

namespace cantrip::error {

exception::exception(const file_pos& pos, const char* err_msg):
    m_pos(pos)
{
    const std::string_view file_name = pos.file_name;
    const std::string row = std::to_string(pos.row);
    const std::string col = std::to_string(pos.col);

    constexpr char row_col_open = '[';
    constexpr char row_col_separator[] = ", ";
    constexpr char row_col_close[] = "]: ";

    const size_t name_len = file_name.size();
    const size_t row_len = row.size();
    const size_t col_len = col.size();

    constexpr size_t row_col_open_len = sizeof(row_col_open);
    // -1 to account for null terminator
    constexpr size_t row_col_separator_len = sizeof(row_col_separator) - 1;
    constexpr size_t row_col_close_len = sizeof(row_col_close) - 1;

    // file pos string representation:
    // "$file_name[$row, $col]: "
    // open sqr bracket len = 1
    // row_str len
    // comma separator len = 2
    // col_str len
    // close square bracket & separator len = 3
    const size_t row_col_open_pos = name_len;
    const size_t row_pos = row_col_open_pos + row_col_open_len;
    const size_t row_col_separator_pos = row_pos + row_len;
    const size_t col_pos = row_col_separator_pos + row_col_separator_len; 
    const size_t row_col_close_pos = col_pos + col_len;

    const size_t pos_len = row_col_close_pos + row_col_close_len;
    const size_t err_msg_len = strlen(err_msg);


    // extra byte for nullchar terminator
    m_err_str = new char[pos_len + err_msg_len + 1];

    // file_pos
    memcpy(m_err_str, file_name.data(), name_len);
    m_err_str[row_col_open_pos] = row_col_open;
    memcpy(m_err_str + row_pos, row.c_str(), row_len);
    memcpy(m_err_str + row_col_separator_pos, row_col_separator, row_col_separator_len);
    memcpy(m_err_str + col_pos, col.c_str(), col_len);
    memcpy(m_err_str + row_col_close_pos, row_col_close, row_col_close_len);

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
