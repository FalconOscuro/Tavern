#ifndef READ_FILE_HPP
#define READ_FILE_HPP

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string_view>

namespace tavern::resource::utility {

inline std::string_view get_file_parent_dir(const std::string_view& s) {
    auto end = s.find_last_of('/');

    if (end == s.npos)
        end = 0;

    else
        end += 1;

    return s.substr(0, end);
}

// WARNING: Do not forget to delete!
// NOTE: Could make raw buffer static, but would mess with multithreading IO ops
[[nodiscard]] inline char* read_file(const char* filename, size_t& size)
{
    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        size = 0;
        return nullptr;
    }
    
    // get file size
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (size <= 0)
        return nullptr;

    // place file contents
    char* raw = new char[size + 1];
    auto read = fread(raw, sizeof(char), size, file);
    assert(read == size);
    (void)read;
    raw[size] = '\0';
    fclose(file);

    return raw;
}

} /* end of namespace tavern::utility */

#endif /* end of define guard READ_FILE_HPP */
