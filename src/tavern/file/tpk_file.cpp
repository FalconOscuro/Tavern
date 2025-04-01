#include "tavern/file/tpk_file.h"

#include <algorithm>
#include <cassert>

#include <boost/log/trivial.hpp>

namespace tavern::file {

tpk_file::tpk_file(const std::string_view tpk_path, const mount_path& file_path, const tpk::file_node* node, const size_t data_start):
    ifile(file_path), m_node(node), m_tpk_path(tpk_path), m_data_start(data_start)
{
    assert(node != nullptr);
}

tpk_file::~tpk_file() {
    close();
}

bool tpk_file::open()
{
    if (is_open())
        return true;

    m_file = fopen(m_tpk_path.c_str(), "rb");
    
    if (is_open() && fseek(m_file, file_start_pos(), SEEK_SET) == 0)
        return true;

    close();
    return false;
}

void tpk_file::close()
{
    if (!is_open())
        return;

    fclose(m_file);
    m_file = NULL;
}

bool tpk_file::eof() const {
    return pos() >= m_node->size || feof(m_file);
}

bool tpk_file::is_open() const {
    return m_file != NULL;
}

char tpk_file::peek_char() const
{
    if (!is_open() || eof())
        return EOF;

    const char c = fgetc(m_file);
    fseek(m_file, -1, SEEK_CUR);
    return c;
}

char tpk_file::get_char() {
    return (!is_open() || eof()) ? EOF : fgetc(m_file);
}

size_t tpk_file::get_str(char* s, const size_t len)
{
    if (!is_open())
        return 0;

    size_t chars_read = 0;

    // cursed for loop
    // c stores character from file
    // loops so long as c not end of file marker and less than len chars read
    for (char c; chars_read < len && ((c = get_char()) != EOF || !eof()); ++chars_read)
        s[chars_read] = c;

    return chars_read;
}

bool tpk_file::seek(const long offset, const origin mode)
{
    if (!is_open())
        return false;

    long seek_pos;
    switch (mode)
    {
    case START:
        seek_pos = 0;
        break;

    case END:
        seek_pos = size() - 1;
        break;

    default:
        seek_pos = pos();
        break;
    }

    const long offset_actual = std::clamp(offset, -(seek_pos), (long)size() - seek_pos);

    return fseek(m_file, m_data_start + offset_actual + seek_pos, SEEK_SET) == 0 && offset_actual == offset;
}

size_t tpk_file::pos() const {
    return is_open() ? ftell(m_file) - file_start_pos() : 0;
}

size_t tpk_file::size() const {
    return m_node->size;
}

} /* namespace tavern::file */
