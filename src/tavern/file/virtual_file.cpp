#include "tavern/file/virtual_file.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace tavern::file {

virtual_file::virtual_file(const char* c, const std::string& path):
    ifile(mount_path("internal", path)), m_data(c)
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

char virtual_file::peek_char() const {
    return eof() ? EOF : m_data[m_pos];
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

long virtual_file::seek(long offset) {
    offset = std::clamp(offset, -long(m_pos), long(size() - m_pos));

    m_pos += offset;
    return offset;
}

void virtual_file::seek_start(const size_t offset) {
    m_pos = 0 + std::min(offset, size());
}

size_t virtual_file::pos() const {
    return m_pos;
}

size_t virtual_file::size() const {
    return strlen(m_data);
}

} /* end of namespace tavern::file */
