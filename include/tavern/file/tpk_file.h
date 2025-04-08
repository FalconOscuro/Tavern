#ifndef TPK_FILE_H
#define TPK_FILE_H

#include "ifile.h"
#include "tpk/tpk.h"

#include <cstdio>

// bias towards populating file buffer whilst reading forwards as a percentage 0-1.0 of file buffer size
// current value is picked arbitrarily
#define TPK_READ_FORWARD_BIAS 0.9

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

    inline bool fully_buffered() const {
        return m_buffer && get_buffer_size() == size();
    }

    inline size_t file_start_pos() const {
        return m_data_start + m_node->start;
    }

    size_t get_buffer_size() const;
    size_t get_buffer_used_size() const;
    size_t get_buffer_abs_pos() const;

    inline size_t read_forward_bias() const {
        return (get_buffer_size() - 1) * TPK_READ_FORWARD_BIAS;
    }

    inline size_t read_backward_bias() const {
        return (get_buffer_size() - 1) - read_forward_bias();
    }

    // pass requested read offset, can be negative for seeking backwards
    bool populate_buffer(long offset) const;

    // reset buffer and read from new file position
    // used for file opens and long seeks
    bool reset_buffer();

    std::FILE* m_file = NULL;

    // WARNING: Lifetime of parent mount owning the node is not garuanteed to be longer than spawned files, could lead to reading invalid address
    const tpk::file_node* const m_node;
    const std::string m_tpk_path;

    const size_t m_data_start;

    size_t m_file_pos = 0;

    // Using custom buffering as standard approach could lead to more than the actual filesize being buffered causing slowdown
    char* m_buffer = nullptr;
    size_t m_buffer_pos = 0;

    // indices for head and tail of used space within buffer
    // NOTE: tail index is un-used memory, so if tail = head then used buffer space = 0
    mutable size_t m_buffer_head = 0;
    mutable size_t m_buffer_tail = 0;
};

} /* namespace tavern::file */

#endif /* end of include guard: TPK_FILE_H */
