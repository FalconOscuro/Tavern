#include "tavern/file/physical_file.h"

#include <boost/log/trivial.hpp>

namespace tavern::file {

physical_file::~physical_file() {
    close();
}

bool physical_file::open()
{
    // file already open
    if (is_open())
        return true;

    m_file = fopen(m_absolute_path.c_str(), "rb");

    return is_open();
}

void physical_file::close()
{
    // file not open
    if (!is_open())
        return;

    fclose(m_file);
    m_file = NULL;
}

bool physical_file::is_open() const {
    return m_file != NULL;
}

bool physical_file::eof() const {
    return feof(m_file);
}

char physical_file::peek_char(const size_t offset) const
{
    const size_t cursor_pos = pos();
    fseek(m_file, offset, SEEK_CUR);

    const char c = fgetc(m_file);

    // do not seek back if peeking eof at offset 0
    if (!(offset == 0 && eof()))
        fseek(m_file, cursor_pos, SEEK_SET);

    return c;
}

char physical_file::get_char() {
    return fgetc(m_file);
}

size_t physical_file::get_str(char* s, const size_t len)
{
    if (!is_open())
        return 0;

    size_t chars_read = 0;

    // cursed for loop
    // c stores character from file
    // loops so long as c not end of file marker and less than len chars read
    for (char c; chars_read < len && ((c = fgetc(m_file)) != EOF || !eof()); ++chars_read)
        s[chars_read] = c;

    return chars_read;
}

bool physical_file::seek(const long offset, const origin mode)
{
    if (!is_open())
        return false;

    int seek_mode;

    switch (mode)
    {
    case START:
        seek_mode = SEEK_SET;
        break;

    case END:
        seek_mode = SEEK_END;
        break;

    default:
        seek_mode = SEEK_CUR;
        break;
    }

    return fseek(m_file, offset, seek_mode) == 0;
}

size_t physical_file::pos() const {
    if (!is_open())
        return 0;

    return ftell(m_file);
}

size_t physical_file::size() const
{
    if (!is_open())
        return 0;

    const size_t pos = ftell(m_file);

    fseek(m_file, 0L, SEEK_END);
    const size_t size = ftell(m_file);

    fseek(m_file, pos, SEEK_SET);
    return size;
}

} /* namespace tavern */
