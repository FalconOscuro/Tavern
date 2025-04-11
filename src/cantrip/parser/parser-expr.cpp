#include "cantrip/parser/ast/expression/core_type.h"
#include "cantrip/parser/parser.h"

#include <flib/exception/unimplemented.hpp>

#include "cantrip/parser/ast/expression/binary.h"
#include "cantrip/parser/ast/expression/unary.h"
#include "cantrip/parser/ast/expression/grouping.h"
#include "cantrip/parser/ast/expression/literal.hpp"
#include "cantrip/parser/ast/expression/identifier.h"
#include "cantrip/parser/ast/expression/call.h"

namespace cantrip {

parser::expr_ptr parser::expression() {
    return cast();
}

/* binary expression parsing in order of precedence */

#define PARSE_BINARY_EXPR(TOKEN_MATCH, EXPR_TYPE, CALL_NEXT)    \
    expr_ptr expr = CALL_NEXT;                          \
                                                                \
    while (match(TOKEN_MATCH)) {                                \
        expr = new ast::binary(                                 \
                EXPR_TYPE,                                      \
                expr, CALL_NEXT                                 \
        );                                                      \
    }                                                           \
                                                                \
    return expr

parser::expr_ptr parser::cast() {
    PARSE_BINARY_EXPR(token::KEYWORD_AS, ast::binary::CAST, logical_or());
}

parser::expr_ptr parser::logical_or() {
    PARSE_BINARY_EXPR(token::BOOL_OR, ast::binary::LOGICAL_OR, logical_and());
}

parser::expr_ptr parser::logical_and() {
    PARSE_BINARY_EXPR(token::BOOL_AND, ast::binary::LOGICAL_AND, logical_not());
}

parser::expr_ptr parser::is_equal() {
    PARSE_BINARY_EXPR(token::IS_EQUAL, ast::binary::IS_EQUAL, not_equal());
}

parser::expr_ptr parser::not_equal() {
    PARSE_BINARY_EXPR(token::NOT_EQUAL, ast::binary::NOT_EQUAL, less_than());
}

parser::expr_ptr parser::less_than() {
    PARSE_BINARY_EXPR(token::LESS_THAN, ast::binary::LESS_THAN, greater_than());
}

parser::expr_ptr parser::greater_than() {
    PARSE_BINARY_EXPR(token::GREATER_THAN, ast::binary::GREATER_THAN, less_than_equal());
}

parser::expr_ptr parser::less_than_equal() {
    PARSE_BINARY_EXPR(token::LESS_THAN_EQUAL, ast::binary::LESS_THAN_EQUAL, greater_than_equal());
}

parser::expr_ptr parser::greater_than_equal() {
    PARSE_BINARY_EXPR(token::GREATER_THAN_EQUAL, ast::binary::GREATER_THAN_EQUAL, subtract());
}

parser::expr_ptr parser::subtract() {
    PARSE_BINARY_EXPR(token::SUBTRACT, ast::binary::SUBTRACT, add());
}

parser::expr_ptr parser::add() {
    PARSE_BINARY_EXPR(token::ADD, ast::binary::ADD, multiply());
}

parser::expr_ptr parser::multiply() {
    PARSE_BINARY_EXPR(token::MULTIPLY, ast::binary::MULTIPLY, divide());
}

parser::expr_ptr parser::divide() {
    PARSE_BINARY_EXPR(token::DIVIDE, ast::binary::DIVIDE, sign());
}

parser::expr_ptr parser::is() {
    PARSE_BINARY_EXPR(token::KEYWORD_IS, ast::binary::IS, assign());
}

parser::expr_ptr parser::assign() {
    PARSE_BINARY_EXPR(token::ASSIGN, ast::binary::ASSIGN, assign_add());
}

parser::expr_ptr parser::assign_add() {
    PARSE_BINARY_EXPR(token::ASSIGN_ADD, ast::binary::ASSIGN_ADD, assign_subtract());
}

parser::expr_ptr parser::assign_subtract() {
    PARSE_BINARY_EXPR(token::ASSIGN_SUBTRACT, ast::binary::ASSIGN_SUBTRACT, assign_multiply());
}

parser::expr_ptr parser::assign_multiply() {
    PARSE_BINARY_EXPR(token::ASSIGN_MULTIPLY, ast::binary::ASSIGN_MULTIPLY, assign_divide());
}

parser::expr_ptr parser::assign_divide() {
    PARSE_BINARY_EXPR(token::ASSIGN_DIVIDE, ast::binary::ASSIGN_DIVIDE, attribute());
}

parser::expr_ptr parser::attribute() {
    PARSE_BINARY_EXPR(token::DOT, ast::binary::ATTRIBUTE, call());
}

#undef PARSE_BINARY_EXPR

parser::expr_ptr parser::logical_not()
{
    if (match(token::BOOL_NOT))
        return new ast::unary( ast::unary::LOGICAL_NOT, logical_not());

    return is_equal();
}

parser::expr_ptr parser::sign()
{
    if (match(token::SUBTRACT))
        return new ast::unary( ast::unary::MINUS, sign());

    return is();
}

parser::expr_ptr parser::call()
{
    expr_ptr expr;

    // branch here to allow for constructor/public function calls
    // NOTE: Prevents builtin types from having constructors
    if (peek() == token::IDENTIFIER && next() == token::BRACKET_L) {
        ast::call* call = new ast::call(peek().data.string);
        m_index += 2;
        call->params = call_param_list();
        expr = call;
    }

    // standard recursive descent
    else
        expr = subscription();

    // handling of object member function calls
    while (peek() == token::DOT && next() == token::IDENTIFIER && m_tokens[m_index + 2] == token::BRACKET_L)
    {
        ast::call* call = new ast::call(next().data.string, expr);
        m_index += 3;
        call->params = call_param_list();
        expr = call;
    }

    return expr;
}

std::vector<parser::expr_ptr> parser::call_param_list()
{
    std::vector<expr_ptr> params;
    const token& t = previous();

    if (peek() != token::BRACKET_R) {
        do {
            params.push_back(expression());
        } while (match(token::COMMA));
    }

    match_or_throw(token::BRACKET_R, t, "Failed to find closing ')'.");
    return params;
}

parser::expr_ptr parser::subscription()
{
    expr_ptr expr = primary();

    if (match(token::BRACKET_SQUARE_L)) {
        const token& t = previous();
        expr = new ast::binary(ast::binary::SUBSCRIPTION, expr, expression());

        match_or_throw(token::BRACKET_SQUARE_R, t, "Could not find closing ']'.");
    }

    return expr;
}

parser::expr_ptr parser::primary()
{
    // grouping
    if (match(token::BRACKET_L)) {
        token t = previous();
        expr_ptr expr = new ast::grouping(expression());

        match_or_throw(token::BRACKET_R, t, "Expected closing ')' but was not found!");
        return expr;
    }

    // literal false
    else if (match(token::BOOL_TRUE))
        return new ast::literal(true);

    // literal true
    else if (match(token::BOOL_FALSE))
        return new ast::literal(false);

    // literal null
    else if (match(token::KEYWORD_NULL))
        return new ast::literal();

    else if (match(token::INTEGER_LITERAL))
        return new ast::literal(previous().data.literal_int);

    else if (match(token::FLOAT_LITERAL))
        return new ast::literal(previous().data.literal_float);

    else if (match(token::STRING_LITERAL))
        return new ast::literal(previous().data.string);

    // core type int
    else if (match(token::TYPE_INTEGER))
        return new ast::core_type(previous().ttype);

    // core type bool
    else if (match(token::TYPE_BOOLEAN))
        return new ast::core_type(previous().ttype);

    // core type float
    else if (match(token::TYPE_FLOAT))
        return new ast::core_type(previous().ttype);

    // core type string
    else if (match(token::TYPE_STRING))
        return new ast::core_type(previous().ttype);

    else if (match(token::IDENTIFIER))
        return new ast::identifier(previous().data.string);

    throw syntax_error(peek(), "Unexpected/invalid token found!");
    return nullptr;
}

} /* namespace cantrip  */
