#ifndef TPK_FILE_H
#define TPK_FILE_H

#include "ifile.h"

#include <cstdio>

namespace tavern {

struct tpk_file_info
{
    char path[255];

    uint64_t start;
    uint32_t size;

    // compression?
    // nested directories over full file paths?
};

class tpk_file : public file
{
public:
    tpk_file(const std::string& tpk_path, const tpk_file_info& info);

    bool open() override;
    void close() override;

    bool is_open() const override;

    char get_char() override;
    size_t get_str(char* s, const size_t len) override;

private:

    std::FILE* m_file = NULL;
    const tpk_file_info m_info;
};

} /* namespace tavern */

#endif /* end of include guard: TPK_FILE_H */
