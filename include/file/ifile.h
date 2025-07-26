#ifndef IFILE_H
#define IFILE_H

#include <cstddef>
#include <memory>

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

    virtual ~ifile() = default;

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual bool is_open() const = 0;
    virtual bool eof() const = 0;

    [[nodiscard]] virtual char peek_char(const size_t offset = 0) const = 0;
    virtual char get_char() = 0;
    [[nodiscard]] virtual size_t get_str(char* s, const size_t len) = 0;

    template<typename T>
    size_t get_data(T* data, size_t count = 1) {
        size_t bytes_read = get_str(reinterpret_cast<char*>(data), sizeof(T) * count);
        return bytes_read / sizeof(T);
    }

    // could be singular seek
    virtual bool seek(const long offset, const origin mode) = 0;
    virtual size_t pos() const = 0;

    virtual size_t size() const = 0;

    const mount_path& get_path() const {
        return m_path;
    }

    const std::string_view get_file_name() const
    {
        const std::string_view path = m_path.get_path();
        size_t file_name_start_pos = path.find_last_of('/');

        // account for root dir with no separator
        // or offset by 1 to first char of filename
        file_name_start_pos = file_name_start_pos == path.npos ? 0 : file_name_start_pos + 1;

        return std::string_view(path.data() + file_name_start_pos, path.size() - file_name_start_pos);
    }

    const std::string_view get_extension() const
    {
        const std::string_view path = m_path.get_path();
        size_t extension_start_pos = path.find_last_of('.');

        extension_start_pos = extension_start_pos == path.npos ? path.size() : extension_start_pos + 1;

        return std::string_view(path.data() + extension_start_pos, path.size() - extension_start_pos);
    }

    // string_view faster than creating fresh string
    const std::string_view get_directory() const
    {
        const std::string_view path = m_path.get_path();
        const auto last = path.find_last_of('/');

        return std::string_view(path.data(), last != path.npos ? last : 0);
    }

protected:

    explicit ifile(const mount_path& path):
        m_path(path)
    {}

private:

    const mount_path m_path;

}; /* end of class ifile */

typedef std::unique_ptr<ifile> file_handle;

//template <typename T>
//bool read_data(T* dest, ifile* file, const size_t count = 1) {
//    const size_t data_size = count * sizeof(T);
//    return file->get_str(reinterpret_cast<char*>(dest), data_size) == data_size;
//}

} /* end of namespace tavern::file */

#endif /* end of define guard IFILE_H */
