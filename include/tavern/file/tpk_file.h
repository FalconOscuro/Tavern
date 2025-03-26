#ifndef TPK_FILE_H
#define TPK_FILE_H

#include "ifile.h"
#include "tpk.h"

#include <cstdint>
#include <cstdio>

namespace tavern::file {

class tpk_file : public ifile
{
public:
    tpk_file(const std::string& tpk_path, const mount_path& file_path, const tpk::file_node* node);
    ~tpk_file();

    bool open() override;
    void close() override;

    bool is_open() const override;
    bool eof() const override;

    [[nodiscard]] char peek_char() const override;
    char get_char() override;
    [[nodiscard]] size_t get_str(char* s, const size_t len) override;

    long seek(long offset) override;
    void seek_start(const size_t offset = 0) override;
    size_t pos() const override;

    size_t size() const override;

private:

    std::FILE* m_file = NULL;
    const tpk::file_node m_node;
};

} /* namespace tavern::file */

#endif /* end of include guard: TPK_FILE_H */
