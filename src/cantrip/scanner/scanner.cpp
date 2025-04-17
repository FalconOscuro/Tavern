#include "cantrip/scanner/scanner.hpp"

#include <string>
#include <cstring>
#include <unordered_map>

#define SIMPLE_TOKEN(tchar, p_type)     \
    case tchar:                         \
        t.type = token_type::p_type;    \
        m_file.pop();                   \
        break;

#define IS_NUMBER(c)        \
    (c >= '0' && c <= '9')

#define IS_ALPHANUMERIC(c)  \
    ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || IS_NUMBER(c))

namespace cantrip {

void scanner::read_tokens() const
{
    if (!m_tokens.empty())
        return;

    eat_whitespace();

    token t;
    //const flib::file::pos fpos = m_file->get_pos();

    switch (m_file.peek_char())
    {
        SIMPLE_TOKEN('{' , BLOCK_START       )
        SIMPLE_TOKEN('}' , BLOCK_END         )
        SIMPLE_TOKEN(';' , STATEMENT_END     )
        SIMPLE_TOKEN(':' , COLON             )
        SIMPLE_TOKEN(',' , COMMA             )
        SIMPLE_TOKEN('(' , BRACKET_L         )
        SIMPLE_TOKEN(')' , BRACKET_R         )
        SIMPLE_TOKEN('[' , BRACKET_SQUARE_L  )
        SIMPLE_TOKEN(']' , BRACKET_SQUARE_R  )

        case '"':
            t = read_string();
            break;

        case '.':
            if (IS_NUMBER(m_file.peek_char(1)))
                t = read_number();

            else {
                t.type = token_type::DOT;
                m_file.pop();
            }
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            t = read_number();
            break;

        case '=':
            if (m_file.peek_char(1) == '=') {
                t.type = IS_EQUAL;
                m_file.pop(2);
            }

            else {
                t.type = ASSIGN;
                m_file.pop();
            }

            break;

        case '-': {
            const char peek = m_file.peek_char(1);

            if (IS_NUMBER(peek))
                t = read_number();

            else if (peek == '=') {
                t.type = ASSIGN_SUBTRACT;
                m_file.pop(2);
            }

            else {
                t.type = SUBTRACT;
                m_file.pop();
            }
        }
            break;


        case '+': {
            const char peek = m_file.peek_char(1);

            if (IS_NUMBER(peek))
                t = read_number();

            else if (peek == '=') {
                t.type = ASSIGN_ADD;
                m_file.pop(2);
            }

            else {
                t.type = ADD;
                m_file.pop();
            }
        }
            break;

        case '*':
            if (m_file.peek_char(1) == '=') {
                t.type = ASSIGN_MULTIPLY;
                m_file.pop(2);
            }

            else {
                t.type = MULTIPLY;
                m_file.pop();
            }
            break;

        case '/':
            if (m_file.peek_char(1) == '=') {
                t.type = ASSIGN_DIVIDE;
                m_file.pop(2);
            }

            else {
                t.type = DIVIDE;
                m_file.pop();
            }
            break;

        case '!':
            if (m_file.peek_char(1) == '=') {
                t.type = NOT_EQUAL;
                m_file.pop(2);
            }

            else {
                t.type = BOOL_NOT;
                m_file.pop();
            }
            break;

        case '<':
            if (m_file.peek_char(1) == '=') {
                t.type = LESS_THAN_EQUAL;
                m_file.pop(2);
            }

            else {
                t.type = LESS_THAN;
                m_file.pop();
            }
            break;

        case '>':
            if (m_file.peek_char(1) == '=') {
                t.type = GREATER_THAN_EQUAL;
                m_file.pop(2);
            }

            else {
                t.type = GREATER_THAN;
                m_file.pop();
            }
            break;

        case '&':
            if (m_file.peek_char(1) == '&') {
                t.type = BOOL_AND;
                m_file.pop(2);
            }

            else {
                t.type = BITWISE_AND;
                m_file.pop();
            }
            break;

        case '|':
            if (m_file.peek_char(1) == '|') {
                t.type = BOOL_OR;
                m_file.pop(2);
            }

            else {
                t.type = BITWISE_OR;
                m_file.pop();
            }
            break;

        case EOF:
            if (m_file.eof())
                t.type = FILE_END;

            else {
                t.type = ERROR;
                static const char EOF_ERROR[] = "File read error!";
                t.data.string = new char[strlen(EOF_ERROR) + 1];
                strcpy(t.data.string, EOF_ERROR);
            }
            break;

        default:
            t = read_complex_token();
            break;
    }

    //t.pos = fpos;
    m_tokens.push(t);
}

void scanner::eat_whitespace() const
{
    // Note: For simplicity syntax is c-like, will later adapt to use python syntax
    while (true) {
        switch (m_file.peek_char())
        {
        case ' ':
        case '\t':
            m_file.pop();
            break;

        case '\n':
            // avoid producing multiple new_line tokens
            // by checking the token queue/last popped token
            if (!((
                    m_last_token_type == NEW_LINE
                    || m_last_token_type == STATEMENT_END
                ) || (!m_tokens.empty() && (
                    m_tokens.back() == NEW_LINE
                    || m_tokens.back() == STATEMENT_END
                ))))
            {
                token t;
                t.type = NEW_LINE;
                t.pos = m_file.pos();
                m_tokens.push(t);
            }
            m_file.pop();
            break;

        case '#':
            eat_comment();
            break;

        default:
            return;
        }
    }
}

void scanner::eat_comment() const {
    // Comment ended by line end OR end of file
    while (m_file.peek_char() != '\n' && !m_file.eof()) {
        m_file.pop();
    }
    m_file.pop();
};

token scanner::read_string() const {
    // Escape sequences unsupported!!!
    token t;
    
    std::string str;
    uint i = 1;

    for (char c = m_file.peek_char(i++); c != '"'; c = m_file.peek_char(i++))
    {
        if (c == EOF) {
            //m_file->get_char(i);
            t.type = ERROR;
            static const char STRING_ERROR[] = "Failed to find string end symbol before end of file!";
            t.data.string = new char[strlen(STRING_ERROR) + 1];
            strcpy(t.data.string, STRING_ERROR);
            return t;
        }

        // Account for strings longer than buffer size
        //else if (i == m_file->buffer_size) {
        //    m_file->get_char(i);
        //    i = 0;
        //}

        str.push_back(c);
    }

    t.type = STRING_LITERAL;
    t.data.string = new char[str.size() + 1];
    m_file.pop(i);

    strcpy(t.data.string, str.c_str());
    return t;
}

token scanner::read_number() const
{
    token t;
    t.type = INTEGER_LITERAL;

    while (IS_NUMBER(m_file.peek_char())) {
        t.data.literal_int = (t.data.literal_int * 10) + (int)(m_file.peek_char() - '0');
        m_file.pop();
    }

    return read_float(t);
}

token scanner::read_float(token token_num) const
{
    // floating point number inferred from decimal point
    if (m_file.peek_char() == '.')
    {
        m_file.pop();
        token_num.type = FLOAT_LITERAL;
        token_num.data.literal_float = token_num.data.literal_int;

        uint n = 10;

        while (IS_NUMBER(m_file.peek_char())) {
            token_num.data.literal_float += (int)(m_file.peek_char() - '0') / (float)n;
            n *= 10;
            m_file.pop();
        }

        if (m_file.peek_char() == 'f')
            m_file.pop();
    }

    // floating point literal specifier
    else if (m_file.peek_char() == 'f') {
        token_num.type = FLOAT_LITERAL;
        token_num.data.literal_float = token_num.data.literal_int;
        m_file.pop();
    }

    // integer literal specifer
    else if (m_file.peek_char() == 'i')
        m_file.pop();

    return token_num;
}

token scanner::read_complex_token() const
{
    token t;

    static const std::unordered_map<std::string, const token_type> KEYWORD_MAP = {
        { "if",         IF               },
        { "elif",       ELSE_IF          },
        { "else",       ELSE             },

        { "for",        FOR              },
        { "while",      WHILE            },

        { "continue",   CONTINUE         },
        { "break",      BREAK            },
        { "return",     RETURN           },

        { "and",        BOOL_AND         },
        { "or",         BOOL_OR          },
        { "not",        BOOL_NOT         },

        { "true",       BOOL_TRUE        },
        { "false",      BOOL_FALSE       },
        { "null",       KEYWORD_NULL     },
        { "self",       KEYWORD_SELF     },

        { "is",         KEYWORD_IS       },
        { "as",         KEYWORD_AS       },

        { "class",      CLASS            },
        { "comp",       COMPONENT        },
        { "func",       FUNCTION         },

        { "virtual",    KEYWORD_VIRTUAL  },
        { "static",     KEYWORD_STATIC   },
        { "const",      KEYWORD_CONST    },

        { "int",        TYPE_INTEGER     },
        { "bool",       TYPE_BOOLEAN     },
        { "float",      TYPE_FLOAT       },
        { "string",     TYPE_STRING      }
    };

    std::string token_str;

    while (IS_ALPHANUMERIC(m_file.peek_char()) || m_file.peek_char() == '_')
    {
        token_str.push_back(m_file.peek_char());
        m_file.pop();
    }

    if (KEYWORD_MAP.find(token_str) != KEYWORD_MAP.end())
        t.type = KEYWORD_MAP.at(token_str);

    // Should be impossible, if this happens something has gone very wrong
    else if (token_str.size() == 0) {
        t.type = ERROR;
        static const char SIZE_ERR[] = "Tried to read complex token but size was 0!";
        t.data.string = new char[strlen(SIZE_ERR) + 1];
        strcpy(t.data.string, SIZE_ERR);
    }

    else {
        t.type = IDENTIFIER;
        t.data.string = new char[token_str.size() + 1];
        strcpy(t.data.string, token_str.c_str());
    }

    return t;
}

} /* namespace cantrip */
