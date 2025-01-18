#ifndef READ_FILE_HPP
#define READ_FILE_HPP

#include <cstdint>
#include <cstdio>

namespace tavern::resource::utility {

// WARNING: Do not forget to delete!
// NOTE: Could make raw buffer static, but would mess with multithreading IO ops
inline char* read_file(const char* filename, uint32_t& size)
{
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        size = 0;
        return nullptr;
    }
    
    // get file size
    fseek(file, 0, SEEK_END);
    size = ftell(file);

    if (size == 0)
        return nullptr;

    // place file contents
    char* raw = new char[size];
    rewind(file);
    fread(raw, sizeof(char), size, file);
    fclose(file);

    return raw;
}

} /* end of namespace tavern::utility */

#endif /* end of define guard READ_FILE_HPP */
