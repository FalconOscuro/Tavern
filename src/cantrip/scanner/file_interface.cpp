#include "cantrip/scanner/file_interface.h"

// using common value of 4 spaces per tab character
#define SPACES_PER_TAB 4

namespace cantrip {

file_interface::file_interface(tavern::file::file_handle& file):
    m_file(file.release())
{
    m_pos.file_name = file->get_path().get_full();
}

char file_interface::get_char() {
    char c = m_file->get_char();

    update_file_pos(c);

    return c;
}

void file_interface::pop(size_t count) {
    for (size_t i = 0; i < count && !m_file->eof(); ++i)
        update_file_pos(m_file->get_char());
}

void file_interface::update_file_pos(char c)
{
    // end of file
    if (c == EOF && m_file->eof())
        return;

    switch (c)
    {
    // enter newline state
    case '\n':
        ++m_pos.row;
        m_pos.col = 0;
        m_pos.indent = 0;
        m_new_line = true;
        return;

    // increment indent if in newline state
    case '\t':
        if (m_new_line)
            m_pos.indent += SPACES_PER_TAB;
        break;

    case ' ':
        if (m_new_line)
            ++m_pos.indent;
        break;

    // drop out of newline state
    default:
        m_new_line = false;
    }

    ++m_pos.col;
}

} /* end of namespace cantrip */
