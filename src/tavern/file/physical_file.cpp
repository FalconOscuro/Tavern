#include "tavern/file/physical_file.h"

namespace tavern {

physical_file::~physical_file() {
    close();
}

bool physical_file::open()
{
    // file already open
    if (is_open())
        return true;

    m_file = fopen(get_filename().c_str(), "rb");

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
    return peek_char() == EOF;
}

char physical_file::peek_char() const
{
    const char c = fgetc(m_file);

    // do not seek back if peeking eof
    if (c != EOF)
        fseek(m_file, -1L, SEEK_CUR);

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
    for (char c; chars_read < len && (c = fgetc(m_file)) != EOF; ++chars_read)
        s[len] = c;

    return chars_read;
}

long physical_file::seek(long offset)
{
    if (!is_open())
        return 0;

    const size_t start = ftell(m_file);
    fseek(m_file, offset, SEEK_CUR);

    return ftell(m_file) - start;
}

void physical_file::seek_start() {
    rewind(m_file);
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
