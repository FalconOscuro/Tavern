#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <cassert>

#include "file_interface.h"

namespace cantrip {

enum token_type {
    FILE_END,
    IDENTIFIER,

    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,

    BOOL_TRUE,
    BOOL_FALSE,
    BOOL_NOT,
    BOOL_AND,
    BOOL_OR,

    BITWISE_AND,
    BITWISE_OR,

    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    IS_EQUAL,
    NOT_EQUAL,

    KEYWORD_NULL,
    KEYWORD_SELF,
    KEYWORD_IS,
    KEYWORD_AS,
    KEYWORD_VIRTUAL,
    KEYWORD_STATIC,
    KEYWORD_CONST,

    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_BOOLEAN,
    TYPE_STRING,

    BRACKET_L,
    BRACKET_R,
    BRACKET_SQUARE_L,
    BRACKET_SQUARE_R,

    IF,
    ELSE_IF,
    ELSE,

    WHILE,
    FOR,

    BREAK,
    CONTINUE,
    RETURN,

    CLASS,
    COMPONENT,
    FUNCTION,

    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,

    ASSIGN,
    ASSIGN_ADD,
    ASSIGN_SUBTRACT,
    ASSIGN_MULTIPLY,
    ASSIGN_DIVIDE,

    BLOCK_START,
    BLOCK_END,
    STATEMENT_END,

    DOT,
    COLON,
    COMMA,
    NEW_LINE,

    ERROR,

    CORE_TYPE_START = TYPE_INTEGER,
    CORE_TYPE_END   = TYPE_STRING,
    ASSIGN_START    = ASSIGN,
    ASSIGN_END      = ASSIGN_DIVIDE,
};

struct token {
    // Memory leak if not destroyed properly with string data
    // Copy by value incomplete, data stored as pointer

    token() = default;
    token(const token& t);

    ~token();

    token_type type = FILE_END;
    // Need file pos tracking
    file_pos pos;

    token& operator=(const token& t);

    bool operator==(token_type p_type) const {
        return type == p_type;
    }

    bool operator!=(token_type p_type) const {
        return type != p_type;
    }

    bool operator>(token_type p_type) const {
           return type > p_type;
    }

    bool operator<(token_type p_type) const {
           return type < p_type;
    }

    bool operator>=(token_type p_type) const {
        return type >= p_type;
    }

    bool operator<=(token_type p_type) const {
        return type <= p_type;
    }

    union {
        int literal_int = 0;
        float literal_float;
        char* string;
    } data;

    std::string to_string() const;

private:
    void clear_data();
};

const char* get_token_type_name(const token& t);

} /* namespace cantrip */

#endif /* end of include guard: TOKEN_H */
