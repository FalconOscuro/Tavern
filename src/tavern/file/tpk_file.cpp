#include "tavern/file/tpk_file.h"

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

    m_file = std::fopen(get_filename().c_str(), "rb");
    // check for failure of fseek?
    std::fseek(m_file, m_info.start, SEEK_SET);

    return is_open();
}

void tpk_file::close()
{
    if (!is_open())
        return;

    std::fclose(m_file);
    m_file = NULL;
}

bool tpk_file::is_open() const {
    return m_file != NULL;
}

char tpk_file::get_char() {
    return get_file_pos() < m_info.size ? std::getc(m_file) : EOF;
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

// returns size + 1 if file not open
size_t tpk_file::get_file_pos() const {
    return is_open() ? std::ftell(m_file) - m_info.start : m_info.size + 1;
}

} /* namespace tavern */
