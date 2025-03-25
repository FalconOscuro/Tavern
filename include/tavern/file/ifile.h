#ifndef IFILE_H
#define IFILE_H

#include <cstddef>
#include <string>

namespace tavern {

class file
{
public:

    explicit file(const std::string& file_path):
        m_filename(file_path)
    {}

    virtual ~file() = default;

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual bool is_open() const = 0;
    virtual bool eof() const = 0;

    [[nodiscard]] virtual char peek_char() const = 0;
    virtual char get_char() = 0;
    [[nodiscard]] virtual size_t get_str(char* s, const size_t len) = 0;

    virtual long seek(long offset) = 0;
    virtual void seek_start() = 0;
    virtual size_t pos() const = 0;

    virtual size_t size() const = 0;

    const std::string& get_filename() const {
        return m_filename;
    }

    // string_view faster than creating fresh string
    const std::string_view get_directory() const
    {
        const auto last = m_filename.find_last_of('/');

        return std::string_view(m_filename.c_str(), last);
    }

private:

    const std::string m_filename;

}; /* end of class file */

} /* end of namespace tavern */

#endif /* end of define guard IFILE_H */
