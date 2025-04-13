#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H

#include <cstdint>
#include <memory>

#include <file/ifile.h>

namespace cantrip {

struct file_pos
{
    uint32_t col;
    uint32_t row;

    uint32_t indent;
}; /* end of struct file_pos */

class file_interface
{
public:

    file_interface(tavern::file::file_handle& file);

    ~file_interface() = default;

    file_interface(const file_interface&) = delete;
    void operator=(const file_interface&) = delete;

    bool open();
    bool is_open() const;

    void close();

    char peek_char(long offset = 0) const;
    char get_char();
    void pop(size_t count = 1);

    bool eof() const;

    const file_pos& pos() const {
        return m_pos;
    }

private:

    void update_file_pos(char c);

    std::unique_ptr<tavern::file::ifile> m_file;

    file_pos m_pos;

}; /* end of class file_interface */

} /* end of namespace cantrip */

#endif /* end of define guard FILE_INTERFACE_H */
