#include "cantrip/scanner/scanner.hpp"

#include <string>
#include <unordered_map>

#define SIMPLE_TOKEN(tchar, p_ttype)  \
    case tchar:                     \
        t.ttype = p_ttype;             \
        m_reader.pop();             \
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
    const flib::file::pos fpos = m_reader.get_pos();

    switch (m_reader.peek())
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
            if (IS_NUMBER(m_reader.peek_char(1)))
                t = read_number();

            else {
                t.ttype = token::type::DOT;
                m_reader.pop();
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
            if (m_reader.peek_char(1) == '=') {
                t.ttype = token::type::IS_EQUAL;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::type::ASSIGN;
                m_reader.pop();
            }

            break;

        case '-': {
            const char peek = m_reader.peek_char(1);

            if (IS_NUMBER(peek))
                t = read_number();

            else if (peek == '=') {
                t.ttype = token::ASSIGN_SUBTRACT;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::SUBTRACT;
                m_reader.pop();
            }
        }
            break;


        case '+': {
            const char peek = m_reader.peek_char(1);

            if (IS_NUMBER(peek))
                t = read_number();

            else if (peek == '=') {
                t.ttype = token::ASSIGN_ADD;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::ADD;
                m_reader.pop();
            }
        }
            break;

        case '*':
            if (m_reader.peek_char(1) == '=') {
                t.ttype = token::ASSIGN_MULTIPLY;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::MULTIPLY;
                m_reader.pop();
            }
            break;

        case '/':
            if (m_reader.peek_char(1) == '=') {
                t.ttype = token::ASSIGN_DIVIDE;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::DIVIDE;
                m_reader.pop();
            }
            break;

        case '!':
            if (m_reader.peek_char(1) == '=') {
                t.ttype = token::NOT_EQUAL;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::BOOL_NOT;
                m_reader.pop();
            }
            break;

        case '<':
            if (m_reader.peek_char(1) == '=') {
                t.ttype = token::LESS_THAN_EQUAL;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::LESS_THAN;
                m_reader.pop();
            }
            break;

        case '>':
            if (m_reader.peek_char(1) == '=') {
                t.ttype = token::GREATER_THAN_EQUAL;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::GREATER_THAN;
                m_reader.pop();
            }
            break;

        case '&':
            if (m_reader.peek_char(1) == '&') {
                t.ttype = token::BOOL_AND;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::BITWISE_AND;
                m_reader.pop();
            }
            break;

        case '|':
            if (m_reader.peek_char(1) == '|') {
                t.ttype = token::BOOL_OR;
                m_reader.pop(2);
            }

            else {
                t.ttype = token::BITWISE_OR;
                m_reader.pop();
            }
            break;

        case EOF:
            if (m_reader.get_state() & flib::file::reader::FILE_END)
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

    t.pos = fpos;
    m_tokens.push(t);
}

void scanner::eat_whitespace() const
{
    // Note: For simplicity syntax is c-like, will later adapt to use python syntax
    while (true) {
        switch (m_reader.peek())
        {
        case ' ':
        case '\t':
            m_reader.pop();
            break;

        case '\n':
            if (!((
                    m_last_token_type == token::NEW_LINE
                    || m_last_token_type == token::STATEMENT_END
                ) || (
                    m_tokens.back() == token::NEW_LINE)
                    || m_tokens.back() == token::STATEMENT_END
                )) {
                token t;
                t.ttype = token::NEW_LINE;
                t.pos = m_reader.get_pos();
                m_tokens.push(t);
            }
            m_reader.pop();
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
    while (m_reader.peek() != '\n' && m_reader.peek() != EOF) {
        m_reader.pop();
    }
    m_reader.pop();
};

token scanner::read_string() const {
    // Escape sequences unsupported!!!
    token t;
    
    std::string str;
    uint i = 1;

    for (char c = m_reader.peek_char(i++); c != '"'; c = m_reader.peek_char(i++))
    {
        if (c == EOF) {
            m_reader.pop(i);
            t.ttype = token::type::ERROR;
            static const char STRING_ERROR[] = "Failed to find string end symbol before end of file!";
            t.data.string = new char[strlen(STRING_ERROR) + 1];
            strcpy(t.data.string, STRING_ERROR);
            return t;
        }

        // Account for strings longer than buffer size
        else if (i == m_reader.buffer_size) {
            m_reader.pop(i);
            i = 0;
        }

        str.push_back(c);
    }

    t.ttype = token::type::STRING_LITERAL;
    t.data.string = new char[str.size() + 1];
    m_reader.pop(i);

    strcpy(t.data.string, str.c_str());
    return t;
}

token scanner::read_number() const
{
    token t;
    t.ttype = token::INTEGER_LITERAL;

    while (IS_NUMBER(m_reader.peek())) {
        t.data.literal_int = (t.data.literal_int * 10) + (m_reader.peek() - '0');
        m_reader.pop();
    }

    return read_float(t);
}

token scanner::read_float(token token_num) const
{
    if (m_reader.peek() == '.')
    {
        m_reader.pop();
        token_num.ttype = token::FLOAT_LITERAL;
        token_num.data.literal_float = token_num.data.literal_int;

        uint n = 10;

        while (IS_NUMBER(m_reader.peek())) {
            token_num.data.literal_float += (m_reader.peek() - '0') / (float)n;
            n *= 10;
            m_reader.pop();
        }

        if (m_reader.peek() == 'f')
            m_reader.pop();
    }

    else if (m_reader.peek() == 'f') {
        token_num.ttype = token::FLOAT_LITERAL;
        token_num.data.literal_float = token_num.data.literal_int;
        m_reader.pop();
    }

    else if (m_reader.peek() == 'i')
        m_reader.pop();

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

    while (IS_ALPHANUMERIC(m_reader.peek()) || m_reader.peek() == '_')
    {
        token_str.push_back(m_reader.peek());
        m_reader.pop();
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
