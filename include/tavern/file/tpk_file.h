#ifndef TPK_FILE_H
#define TPK_FILE_H

#include "ifile.h"

#include <cstdint>
#include <cstdio>

namespace tavern::file {

struct tpk_file_info
{
    char path[255];

    uint64_t start;
    uint32_t size;

    // compression?
    // nested directories over full file paths?
};

class tpk_file : public ifile
{
public:
    tpk_file(const std::string& tpk_path, const tpk_file_info& info);
    ~tpk_file();

    bool open() override;
    void close() override;

    bool is_open() const override;
    bool eof() const override;

    [[nodiscard]] char peek_char() const override;
    char get_char() override;
    [[nodiscard]] size_t get_str(char* s, const size_t len) override;

    long seek(long offset) override;
    void seek_start() override;
    size_t pos() const override;

    size_t size() const override;

private:

    std::FILE* m_file = NULL;
    const tpk_file_info m_info;
};

} /* namespace tavern::file */

#endif /* end of include guard: TPK_FILE_H */
