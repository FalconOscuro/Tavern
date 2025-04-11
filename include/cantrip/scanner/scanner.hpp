#ifndef SCANNER_H
#define SCANNER_H

#include <memory>
#include <queue>

#include <file/ifile.h>

#include "token.h"

namespace cantrip {

class scanner {
public:

    scanner(tavern::file::ifile* file): m_file(file) {
        assert(file != nullptr);
    }

    ~scanner() = default;

    bool open() {
        return m_file->open();
    }

    bool is_open() const {
        return m_file->is_open();
    }

    void close() {
        m_file->close();
    }

    const token& peek() const {
        read_tokens();
        return m_tokens.front();
    }

    void pop() {
        read_tokens();
        
        if (m_tokens.size()) {
            m_last_token_type = m_tokens.front().ttype;
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

    mutable std::unique_ptr<tavern::file::ifile> m_file;

    mutable std::queue<token> m_tokens;
    token::type m_last_token_type;
};

} /* namespace cantrip */

#endif /* end of include guard: SCANNER_H */
