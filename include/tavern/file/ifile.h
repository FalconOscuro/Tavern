#ifndef IFILE_H
#define IFILE_H

#include <cstddef>
#include <string>

#include "path.h"

namespace tavern::file {

enum origin {
    START,
    CURRENT,
    END
};

class ifile
{
public:

    explicit ifile(const mount_path& path):
        m_path(path)
    {}

    virtual ~ifile() = default;

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual bool is_open() const = 0;
    virtual bool eof() const = 0;

    [[nodiscard]] virtual char peek_char() const = 0;
    virtual char get_char() = 0;
    [[nodiscard]] virtual size_t get_str(char* s, const size_t len) = 0;

    // could be singular seek
    virtual bool seek(const long offset, const origin mode) = 0;
    virtual size_t pos() const = 0;

    virtual size_t size() const = 0;

    const mount_path& get_path() {
        return m_path;
    }

    // string_view faster than creating fresh string
    const std::string_view get_directory() const
    {
        const std::string_view path = m_path.get_path();
        const auto last = path.find_last_of('/');

        return std::string_view(path.data(), last);
    }

private:

    const mount_path m_path;

}; /* end of class ifile */

template <typename T>
bool read_data(T* dest, ifile* file, const size_t count = 1) {
    const size_t data_size = count * sizeof(T);
    return file->get_str(reinterpret_cast<char*>(dest), data_size) == data_size;
}

} /* end of namespace tavern::file */

#endif /* end of define guard IFILE_H */
