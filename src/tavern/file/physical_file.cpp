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

    m_file = std::fopen(get_filename().c_str(), "r");

    return is_open();
}

void physical_file::close()
{
    // file not open
    if (!is_open())
        return;

    std::fclose(m_file);
    m_file = NULL;
}

bool physical_file::is_open() const {
    return m_file != NULL;
}

char physical_file::get_char() {
    return std::getc(m_file);
}

size_t physical_file::get_str(char* s, const size_t len)
{
    if (!is_open())
        return 0;

    size_t chars_read = 0;

    // cursed for loop
    // c stores character from file
    // loops so long as c not end of file marker and less than len chars read
    for (char c; chars_read < len && (c = std::getc(m_file)) != EOF; ++chars_read)
        s[len] = c;

    return chars_read;
}

} /* namespace tavern */
