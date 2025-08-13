#ifndef SCANNER_H
#define SCANNER_H

#include <queue>

#include "file_interface.h"
#include "token.h"

namespace cantrip {

class scanner {
public:

    scanner(std::vector<tavern::file::file_handle>&& files);

    ~scanner() = default;

    bool open() {
        return open_file_internal();
    }

    bool is_open() const {
        return !all_files_read() && current_file().is_open();
    }

    void close() {
        close_file_internal();
    }

    const token& peek() const {
        read_tokens();
        return m_tokens.front();
    }

    void pop() {
        read_tokens();
        
        if (m_tokens.size()) {
            m_last_token_type = m_tokens.front().type;
            m_tokens.pop();
        }
    }

private:

    void read_tokens() const;
    void eat_whitespace() const;
    void eat_comment() const;

    token read_string() const;
    token read_number() const;
    token read_float(token token_num) const;
    token read_complex_token() const;

    // hack to get around constness specifier for internal behaviour when handling multiple files
    inline bool open_file_internal() const {
        return !all_files_read() && current_file().open();
    }

    // as above
    inline void close_file_internal() const {
        if (!all_files_read())
            current_file().close();
    }

    file_interface& current_file() const {
        return m_files[m_current_file_index];
    }

    inline bool all_files_read() const {
        return m_current_file_index >= m_files.size();
    }

    mutable std::vector<file_interface> m_files;
    mutable size_t m_current_file_index = 0;

    mutable std::queue<token> m_tokens;
    token_type m_last_token_type;
};

} /* namespace cantrip */

#endif /* end of include guard: SCANNER_H */
