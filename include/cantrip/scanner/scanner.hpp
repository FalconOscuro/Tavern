#ifndef SCANNER_H
#define SCANNER_H

#include <flib/filereader.hpp>
#include <queue>

#include "token.h"

namespace cantrip {

class scanner {
public:

    scanner()
    {}

    scanner(const char* filename): m_reader(filename)
    {}


    bool open(const char* filename) {
        return m_reader.open(filename);
    }

    bool is_open() const {
        return m_reader.get_state() & flib::file::reader::OPEN;
    }

    void close() {
        m_reader.close();
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

    mutable flib::file::reader m_reader;

    mutable std::queue<token> m_tokens;
    token::type m_last_token_type;
};

} /* namespace cantrip */

#endif /* end of include guard: SCANNER_H */
