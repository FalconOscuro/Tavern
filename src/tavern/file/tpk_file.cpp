#include "tavern/file/tpk_file.h"

#include <algorithm>

namespace tavern {

tpk_file::tpk_file(const std::string& tpk_path, const tpk_file_info& info):
    file(tpk_path), m_info(info)
{}

tpk_file::~tpk_file() {
    close();
}

bool tpk_file::open()
{
    if (is_open())
        return true;

    m_file = fopen(get_filename().c_str(), "rb");
    // check for failure of fseek?
    fseek(m_file, m_info.start, SEEK_SET);

    return is_open();
}

void tpk_file::close()
{
    if (!is_open())
        return;

    fclose(m_file);
    m_file = NULL;
}

bool tpk_file::eof() const {
    return pos() >= m_info.size;
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
    for (char c; chars_read < len && (c = get_char()) != EOF; ++chars_read)
        s[len] = c;

    return chars_read;
}

long tpk_file::seek(long offset)
{
    if (!is_open())
        return 0;

    const long curr = pos();

    offset = std::clamp(offset, -curr, long(size() - curr));
    fseek(m_file, offset, SEEK_CUR);

    return offset;
}

void tpk_file::seek_start()
{
    if (!is_open())
        return;

    fseek(m_file, -long(pos()), SEEK_CUR);
}

// returns size + 1 if file not open
size_t tpk_file::pos() const {
    return is_open() ? ftell(m_file) - m_info.start : m_info.size + 1;
}

size_t tpk_file::size() const {
    return m_info.size;
}

} /* namespace tavern */
