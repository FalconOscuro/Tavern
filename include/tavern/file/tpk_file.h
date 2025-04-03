#ifndef TPK_FILE_H
#define TPK_FILE_H

#include "ifile.h"
#include "tpk/tpk.h"

#include <cstdio>

namespace tavern::file {

class tpk_file : public ifile
{
public:
    tpk_file(const std::string_view tpk_path, const mount_path& file_path, const tpk::file_node* node, const size_t data_start);
    ~tpk_file();

    bool open() override;
    void close() override;

    bool is_open() const override;
    bool eof() const override;

    [[nodiscard]] char peek_char(const size_t offset = 0) const override;
    char get_char() override;
    [[nodiscard]] size_t get_str(char* s, const size_t len) override;

    bool seek(const long offset, const origin mode) override;
    size_t pos() const override;

    size_t size() const override;

private:

    inline size_t file_start_pos() const {
        return m_data_start + m_node->start;
    }

    std::FILE* m_file = NULL;
    // unsafe?
    const tpk::file_node* const m_node;
    const std::string m_tpk_path;

    const size_t m_data_start;
};

} /* namespace tavern::file */

#endif /* end of include guard: TPK_FILE_H */
