#include "cantrip/scanner/scanner.hpp"

#include <string>
#include <cstring>
#include <unordered_map>

#define SIMPLE_TOKEN(tchar, p_ttype)    \
    case tchar:                         \
        t.ttype = p_ttype;              \
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
        SIMPLE_TOKEN('{' , token::BLOCK_START       )
        SIMPLE_TOKEN('}' , token::BLOCK_END         )
        SIMPLE_TOKEN(';' , token::STATEMENT_END     )
        SIMPLE_TOKEN(':' , token::COLON             )
        SIMPLE_TOKEN(',' , token::COMMA             )
        SIMPLE_TOKEN('(' , token::BRACKET_L         )
        SIMPLE_TOKEN(')' , token::BRACKET_R         )
        SIMPLE_TOKEN('[' , token::BRACKET_SQUARE_L  )
        SIMPLE_TOKEN(']' , token::BRACKET_SQUARE_R  )

        case '"':
            t = read_string();
            break;

        case '.':
            if (IS_NUMBER(m_file.peek_char(1)))
                t = read_number();

            else {
                t.ttype = token::type::DOT;
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
                t.ttype = token::type::IS_EQUAL;
                m_file.pop(2);
            }

            else {
                t.ttype = token::type::ASSIGN;
                m_file.pop();
            }

            break;

        case '-': {
            const char peek = m_file.peek_char(1);

            if (IS_NUMBER(peek))
                t = read_number();

            else if (peek == '=') {
                t.ttype = token::ASSIGN_SUBTRACT;
                m_file.pop(2);
            }

            else {
                t.ttype = token::SUBTRACT;
                m_file.pop();
            }
        }
            break;


        case '+': {
            const char peek = m_file.peek_char(1);

            if (IS_NUMBER(peek))
                t = read_number();

            else if (peek == '=') {
                t.ttype = token::ASSIGN_ADD;
                m_file.pop(2);
            }

            else {
                t.ttype = token::ADD;
                m_file.pop();
            }
        }
            break;

        case '*':
            if (m_file.peek_char(1) == '=') {
                t.ttype = token::ASSIGN_MULTIPLY;
                m_file.pop(2);
            }

            else {
                t.ttype = token::MULTIPLY;
                m_file.pop();
            }
            break;

        case '/':
            if (m_file.peek_char(1) == '=') {
                t.ttype = token::ASSIGN_DIVIDE;
                m_file.pop(2);
            }

            else {
                t.ttype = token::DIVIDE;
                m_file.pop();
            }
            break;

        case '!':
            if (m_file.peek_char(1) == '=') {
                t.ttype = token::NOT_EQUAL;
                m_file.pop(2);
            }

            else {
                t.ttype = token::BOOL_NOT;
                m_file.pop();
            }
            break;

        case '<':
            if (m_file.peek_char(1) == '=') {
                t.ttype = token::LESS_THAN_EQUAL;
                m_file.pop(2);
            }

            else {
                t.ttype = token::LESS_THAN;
                m_file.pop();
            }
            break;

        case '>':
            if (m_file.peek_char(1) == '=') {
                t.ttype = token::GREATER_THAN_EQUAL;
                m_file.pop(2);
            }

            else {
                t.ttype = token::GREATER_THAN;
                m_file.pop();
            }
            break;

        case '&':
            if (m_file.peek_char(1) == '&') {
                t.ttype = token::BOOL_AND;
                m_file.pop(2);
            }

            else {
                t.ttype = token::BITWISE_AND;
                m_file.pop();
            }
            break;

        case '|':
            if (m_file.peek_char(1) == '|') {
                t.ttype = token::BOOL_OR;
                m_file.pop(2);
            }

            else {
                t.ttype = token::BITWISE_OR;
                m_file.pop();
            }
            break;

        case EOF:
            if (m_file.eof())
                t.ttype = token::FILE_END;

            else {
                t.ttype = token::ERROR;
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
                    m_last_token_type == token::NEW_LINE
                    || m_last_token_type == token::STATEMENT_END
                ) || (!m_tokens.empty() && (
                    m_tokens.back() == token::NEW_LINE
                    || m_tokens.back() == token::STATEMENT_END
                ))))
            {
                token t;
                t.ttype = token::NEW_LINE;
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
            t.ttype = token::type::ERROR;
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

    t.ttype = token::type::STRING_LITERAL;
    t.data.string = new char[str.size() + 1];
    m_file.pop(i);

    strcpy(t.data.string, str.c_str());
    return t;
}

token scanner::read_number() const
{
    token t;
    t.ttype = token::INTEGER_LITERAL;

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
        token_num.ttype = token::FLOAT_LITERAL;
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
        token_num.ttype = token::FLOAT_LITERAL;
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

    static const std::unordered_map<std::string, const token::type> KEYWORD_MAP = {
        { "if",         token::IF               },
        { "elif",       token::ELSE_IF          },
        { "else",       token::ELSE             },

        { "for",        token::FOR              },
        { "while",      token::WHILE            },

        { "continue",   token::CONTINUE         },
        { "break",      token::BREAK            },
        { "return",     token::RETURN           },

        { "and",        token::BOOL_AND         },
        { "or",         token::BOOL_OR          },
        { "not",        token::BOOL_NOT         },

        { "true",       token::BOOL_TRUE        },
        { "false",      token::BOOL_FALSE       },
        { "null",       token::KEYWORD_NULL     },
        { "self",       token::KEYWORD_SELF     },

        { "is",         token::KEYWORD_IS       },
        { "as",         token::KEYWORD_AS       },

        { "class",      token::CLASS            },
        { "comp",       token::COMPONENT        },
        { "func",       token::FUNCTION         },

        { "virtual",    token::KEYWORD_VIRTUAL  },
        { "static",     token::KEYWORD_STATIC   },
        { "const",      token::KEYWORD_CONST    },

        { "int",        token::TYPE_INTEGER     },
        { "bool",       token::TYPE_BOOLEAN     },
        { "float",      token::TYPE_FLOAT       },
        { "string",     token::TYPE_STRING      }
    };

    std::string token_str;

    while (IS_ALPHANUMERIC(m_file.peek_char()) || m_file.peek_char() == '_')
    {
        token_str.push_back(m_file.peek_char());
        m_file.pop();
    }

    if (KEYWORD_MAP.find(token_str) != KEYWORD_MAP.end())
        t.ttype = KEYWORD_MAP.at(token_str);

    // Should be impossible, if this happens something has gone very wrong
    else if (token_str.size() == 0) {
        t.ttype = token::ERROR;
        static const char SIZE_ERR[] = "Tried to read complex token but size was 0!";
        t.data.string = new char[strlen(SIZE_ERR) + 1];
        strcpy(t.data.string, SIZE_ERR);
    }

    else {
        t.ttype = token::IDENTIFIER;
        t.data.string = new char[token_str.size() + 1];
        strcpy(t.data.string, token_str.c_str());
    }

    return t;
}

} /* namespace cantrip */
