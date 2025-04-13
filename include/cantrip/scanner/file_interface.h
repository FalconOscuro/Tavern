#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H

#include <cstdint>
#include <memory>

#include <file/ifile.h>

namespace cantrip {

struct file_pos
{
    uint32_t col = 0;
    uint32_t row = 0;

    uint32_t indent = 0;

    // NOTE: Equality operators do not account for indent, they only compare the absolute file position
    bool operator==(const file_pos& rhs) const;
    bool operator!=(const file_pos& rhs) const;
}; /* end of struct file_pos */

class file_interface
{
public:

    file_interface(tavern::file::file_handle& file);

    ~file_interface() = default;

    file_interface(const file_interface&) = delete;
    void operator=(const file_interface&) = delete;

    inline bool open() {
        return m_file->open();
    };

    inline bool is_open() const {
        return m_file->is_open();
    };

    inline void close() {
        m_file->close();
    }

    inline char peek_char(long offset = 0) const {
        return m_file->peek_char(offset);
    }

    char get_char();
    void pop(size_t count = 1);

    inline bool eof() const {
        return m_file->eof();
    }

    inline const file_pos& pos() const {
        return m_pos;
    }

private:

    void update_file_pos(char c);

    std::unique_ptr<tavern::file::ifile> m_file;

    file_pos m_pos;

    bool m_new_line = true;

}; /* end of class file_interface */

} /* end of namespace cantrip */

#endif /* end of define guard FILE_INTERFACE_H */
