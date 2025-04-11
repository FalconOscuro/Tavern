#include "cantrip/scanner/token.h"

#include <cstring>

namespace cantrip {

token::token(const token& t) {
    ttype = t.ttype;
    //pos = t.pos;

    switch (ttype)
    {
        case IDENTIFIER:
        case STRING_LITERAL:
        case ERROR:
            data.string = new char[strlen(t.data.string) + 1];
            strcpy(data.string, t.data.string);
            break;

        case INTEGER_LITERAL:
            data.literal_int = t.data.literal_int;
            break;

        case FLOAT_LITERAL:
            data.literal_float = t.data.literal_float;
            break;

        default:
            break;
    }
}

token::~token() {
    clear_data();
}

token& token::operator=(const token& t) {
    clear_data();

    ttype = t.ttype;
    //pos = t.pos;

    switch (ttype) {
        case IDENTIFIER:
        case STRING_LITERAL:
        case ERROR:
            data.string = new char[strlen(t.data.string) + 1];
            strcpy(data.string, t.data.string);
            break;

        case INTEGER_LITERAL:
            data.literal_int = t.data.literal_int;
            break;

        case FLOAT_LITERAL:
            data.literal_float = t.data.literal_float;
            break;

        default:
            break;
    }

    return *this;
}

#define TOKEN_STRING_SW_CASE(t) \
    case t:                     \
        //str = "["  + std::to_string(pos.row) + ", " + std::to_string(pos.col) + "]: " #t;

std::string token::to_string() const
{
    std::string str;

    switch (ttype)
    {
        TOKEN_STRING_SW_CASE(IDENTIFIER)
            str += ": ";
            str.append(data.string);
            break;

        TOKEN_STRING_SW_CASE(INTEGER_LITERAL)
            str += ": " + std::to_string(data.literal_int);
            break;

        TOKEN_STRING_SW_CASE(FLOAT_LITERAL)
            str += ": " + std::to_string(data.literal_float);
            break;

        TOKEN_STRING_SW_CASE(STRING_LITERAL)
            str += ": ";
            str.append(data.string);
            break;

        TOKEN_STRING_SW_CASE(BOOL_TRUE)
            break;

        TOKEN_STRING_SW_CASE(BOOL_FALSE)
            break;

        TOKEN_STRING_SW_CASE(BOOL_NOT)
            break;

        TOKEN_STRING_SW_CASE(BOOL_AND)
            break;

        TOKEN_STRING_SW_CASE(BOOL_OR)
            break;

        TOKEN_STRING_SW_CASE(BITWISE_AND)
            break;

        TOKEN_STRING_SW_CASE(BITWISE_OR)
            break;

        TOKEN_STRING_SW_CASE(LESS_THAN)
            break;

        TOKEN_STRING_SW_CASE(GREATER_THAN)
            break;

        TOKEN_STRING_SW_CASE(LESS_THAN_EQUAL)
            break;

        TOKEN_STRING_SW_CASE(GREATER_THAN_EQUAL)
            break;

        TOKEN_STRING_SW_CASE(IS_EQUAL)
            break;

        TOKEN_STRING_SW_CASE(NOT_EQUAL)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_NULL)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_SELF)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_IS)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_AS)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_VIRTUAL)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_STATIC)
            break;

        TOKEN_STRING_SW_CASE(KEYWORD_CONST)
            break;

        TOKEN_STRING_SW_CASE(TYPE_INTEGER)
            break;

        TOKEN_STRING_SW_CASE(TYPE_BOOLEAN)
            break;

        TOKEN_STRING_SW_CASE(TYPE_FLOAT)
            break;

        TOKEN_STRING_SW_CASE(TYPE_STRING)
            break;

        TOKEN_STRING_SW_CASE(BRACKET_L)
            break;

        TOKEN_STRING_SW_CASE(BRACKET_R)
            break;

        TOKEN_STRING_SW_CASE(BRACKET_SQUARE_L)
            break;

        TOKEN_STRING_SW_CASE(BRACKET_SQUARE_R)
            break;

        TOKEN_STRING_SW_CASE(IF)
            break;

        TOKEN_STRING_SW_CASE(ELSE_IF)
            break;

        TOKEN_STRING_SW_CASE(ELSE)
            break;

        TOKEN_STRING_SW_CASE(WHILE)
            break;

        TOKEN_STRING_SW_CASE(FOR)
            break;

        TOKEN_STRING_SW_CASE(BREAK)
            break;

        TOKEN_STRING_SW_CASE(CONTINUE)
            break;

        TOKEN_STRING_SW_CASE(RETURN)
            break;

        TOKEN_STRING_SW_CASE(CLASS)
            break;

        TOKEN_STRING_SW_CASE(COMPONENT)
            break;

        TOKEN_STRING_SW_CASE(FUNCTION)
            break;

        TOKEN_STRING_SW_CASE(ADD)
            break;

        TOKEN_STRING_SW_CASE(SUBTRACT)
            break;

        TOKEN_STRING_SW_CASE(MULTIPLY)
            break;

        TOKEN_STRING_SW_CASE(DIVIDE)
            break;

        TOKEN_STRING_SW_CASE(ASSIGN)
            break;

        TOKEN_STRING_SW_CASE(ASSIGN_ADD)
            break;

        TOKEN_STRING_SW_CASE(ASSIGN_SUBTRACT)
            break;

        TOKEN_STRING_SW_CASE(ASSIGN_MULTIPLY)
            break;

        TOKEN_STRING_SW_CASE(ASSIGN_DIVIDE)
            break;

        TOKEN_STRING_SW_CASE(BLOCK_START)
            break;

        TOKEN_STRING_SW_CASE(BLOCK_END)
            break;

        TOKEN_STRING_SW_CASE(STATEMENT_END)
            break;

        TOKEN_STRING_SW_CASE(DOT)
            break;

        TOKEN_STRING_SW_CASE(COLON)
            break;

        TOKEN_STRING_SW_CASE(COMMA)
            break;

        TOKEN_STRING_SW_CASE(NEW_LINE)
            break;

        TOKEN_STRING_SW_CASE(ERROR)
            str += ": ";
            str.append(data.string);
            break;

        TOKEN_STRING_SW_CASE(FILE_END)
            break;
    }

    return str;
}

#undef TOKEN_STRING_SW_CASE

void token::clear_data() {
    switch (ttype)
    {
        case IDENTIFIER:
        case STRING_LITERAL:
        case ERROR:
            delete data.string;
            data.literal_int = 0;
            break;

        default:
            break;
    }
}

const char* get_token_coretype_name(token::type t)
{
    assert(t >= token::CORE_TYPE_START && t <= token::CORE_TYPE_END &&
           "Token type must be a core type to get coretype name.");

    static const char* CORE_TYPE_NAMES[] = {
        "int", "float", "bool", "string"
    };

    return CORE_TYPE_NAMES[t - token::CORE_TYPE_START];
}

} /* namespace cantrip */
