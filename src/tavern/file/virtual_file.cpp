#include "tavern/file/virtual_file.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace tavern::file {

virtual_file::virtual_file(const char* c, const std::string_view path):
    ifile(mount_path("internal", path)), m_data(c), m_size(strlen(c))
{
    assert(c != nullptr);
}

bool virtual_file::open() {
    return true;
}

void virtual_file::close() {
    m_pos = 0;
}

bool virtual_file::is_open() const {
    return true;
}

bool virtual_file::eof() const {
    return m_pos >= size();
}

char virtual_file::peek_char(const size_t offset) const
{
    if (eof() || m_pos + offset >= size())
        return EOF;

    return m_data[m_pos + offset];
}

char virtual_file::get_char() {
    return eof() ? EOF : m_data[m_pos++];
}

size_t virtual_file::get_str(char* s, const size_t len)
{
    size_t chars_read = 0;

    for (char c; chars_read < len && (c = get_char()) != EOF; ++chars_read)
        s[chars_read] = c;

    return chars_read;
}

bool virtual_file::seek(const long offset, const origin mode)
{
    switch (mode)
    {
    case START:
        m_pos = 0;
        break;

    case END:
        m_pos = size() - 1;
        break;

    default:
        break;
    }

    const long offset_actual = std::clamp<long>(offset, -long(m_pos), size() - m_pos);

    m_pos += offset_actual;
    return offset_actual == offset;
}

size_t virtual_file::pos() const {
    return m_pos;
}

size_t virtual_file::size() const {
    return m_size;
}

} /* end of namespace tavern::file */
