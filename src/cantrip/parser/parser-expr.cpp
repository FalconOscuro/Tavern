#include "cantrip/parser/parser.h"

#include <flib/exception/unimplemented.hpp>

#include "cantrip/parser/ast/expression/binary.h"
#include "cantrip/parser/ast/expression/unary.h"
#include "cantrip/parser/ast/expression/grouping.h"
#include "cantrip/parser/ast/expression/literal.hpp"
#include "cantrip/parser/ast/expression/identifier.h"
#include "cantrip/parser/ast/expression/call.h"
#include "cantrip/parser/ast/expression/core_type.h"

namespace cantrip {

parser::expr_ptr parser::expression() {
    return cast();
}

/* binary expression parsing in order of precedence */

#define PARSE_BINARY_EXPR(TOKEN_MATCH, EXPR_TYPE, CALL_NEXT)    \
    expr_ptr expr = CALL_NEXT;                                  \
                                                                \
    while (match(token_type::TOKEN_MATCH)) {                    \
        expr = std::make_unique<ast::binary>(                   \
                ast::binary_operator::EXPR_TYPE,                \
                expr.release(), CALL_NEXT.release()             \
        );                                                      \
    }                                                           \
                                                                \
    return expr

parser::expr_ptr parser::cast() {
    PARSE_BINARY_EXPR(KEYWORD_AS, CAST, logical_or());
}

parser::expr_ptr parser::logical_or() {
    PARSE_BINARY_EXPR(BOOL_OR, LOGICAL_OR, logical_and());
}

parser::expr_ptr parser::logical_and() {
    PARSE_BINARY_EXPR(BOOL_AND, LOGICAL_AND, logical_not());
}

parser::expr_ptr parser::is_equal() {
    PARSE_BINARY_EXPR(IS_EQUAL, IS_EQUAL, not_equal());
}

parser::expr_ptr parser::not_equal() {
    PARSE_BINARY_EXPR(NOT_EQUAL, NOT_EQUAL, less_than());
}

parser::expr_ptr parser::less_than() {
    PARSE_BINARY_EXPR(LESS_THAN, LESS_THAN, greater_than());
}

parser::expr_ptr parser::greater_than() {
    PARSE_BINARY_EXPR(GREATER_THAN, GREATER_THAN, less_than_equal());
}

parser::expr_ptr parser::less_than_equal() {
    PARSE_BINARY_EXPR(LESS_THAN_EQUAL, LESS_THAN_EQUAL, greater_than_equal());
}

parser::expr_ptr parser::greater_than_equal() {
    PARSE_BINARY_EXPR(GREATER_THAN_EQUAL, GREATER_THAN_EQUAL, subtract());
}

parser::expr_ptr parser::subtract() {
    PARSE_BINARY_EXPR(SUBTRACT, SUBTRACT, add());
}

parser::expr_ptr parser::add() {
    PARSE_BINARY_EXPR(ADD, ADD, multiply());
}

parser::expr_ptr parser::multiply() {
    PARSE_BINARY_EXPR(MULTIPLY, MULTIPLY, divide());
}

parser::expr_ptr parser::divide() {
    PARSE_BINARY_EXPR(DIVIDE, DIVIDE, sign());
}

parser::expr_ptr parser::is() {
    PARSE_BINARY_EXPR(KEYWORD_IS, IS, assign());
}

parser::expr_ptr parser::assign() {
    PARSE_BINARY_EXPR(ASSIGN, ASSIGN, assign_add());
}

parser::expr_ptr parser::assign_add() {
    PARSE_BINARY_EXPR(ASSIGN_ADD, ASSIGN_ADD, assign_subtract());
}

parser::expr_ptr parser::assign_subtract() {
    PARSE_BINARY_EXPR(ASSIGN_SUBTRACT, ASSIGN_SUBTRACT, assign_multiply());
}

parser::expr_ptr parser::assign_multiply() {
    PARSE_BINARY_EXPR(ASSIGN_MULTIPLY, ASSIGN_MULTIPLY, assign_divide());
}

parser::expr_ptr parser::assign_divide() {
    PARSE_BINARY_EXPR(ASSIGN_DIVIDE, ASSIGN_DIVIDE, attribute());
}

parser::expr_ptr parser::attribute() {
    PARSE_BINARY_EXPR(DOT, ATTRIBUTE, call());
}

#undef PARSE_BINARY_EXPR

parser::expr_ptr parser::logical_not()
{
    if (match(token_type::BOOL_NOT))
        return std::make_unique<ast::unary>(ast::unary_operator::LOGICAL_NOT, logical_not().release());

    return is_equal();
}

parser::expr_ptr parser::sign()
{
    if (match(token_type::SUBTRACT))
        return std::make_unique<ast::unary>(ast::unary_operator::MINUS, sign().release());

    return is();
}

parser::expr_ptr parser::call()
{
    expr_ptr expr = nullptr;

    // branch here to allow for constructor/public function calls
    // NOTE: Prevents builtin types from having constructors
    if (peek() == token_type::IDENTIFIER && next() == token_type::BRACKET_L) {
        u_ptr<ast::call> call = std::make_unique<ast::call>(peek().data.string);
        m_index += 2;
        call->params = call_param_list();
        expr = expr_ptr(call.release());
    }

    // standard recursive descent
    else
        expr = subscription();

    // handling of object member function calls
    while (peek() == token_type::DOT && next() == token_type::IDENTIFIER && m_tokens[m_index + 2] == token_type::BRACKET_L)
    {
        u_ptr<ast::call> call = std::make_unique<ast::call>(next().data.string, expr.release());
        m_index += 3;
        call->params = call_param_list();
        expr = expr_ptr(call.release());
    }

    return expr;
}

std::vector<parser::expr_ptr> parser::call_param_list()
{
    std::vector<expr_ptr> params;
    const token& t = previous();

    if (peek() != token_type::BRACKET_R) {
        do {
            params.push_back(expression());
        } while (match(token_type::COMMA));
    }

    match_or_throw(token_type::BRACKET_R, t, "Failed to find closing ')'.");
    return params;
}

parser::expr_ptr parser::subscription()
{
    expr_ptr expr = primary();

    if (match(token_type::BRACKET_SQUARE_L)) {
        const token& t = previous();
        expr = std::make_unique<ast::binary>(ast::binary_operator::SUBSCRIPTION, expr.release(), expression().release());

        match_or_throw(token_type::BRACKET_SQUARE_R, t, "Could not find closing ']'.");
    }

    return expr;
}

parser::expr_ptr parser::primary()
{
    // grouping
    if (match(token_type::BRACKET_L)) {
        token t = previous();
        expr_ptr expr = std::make_unique<ast::grouping>(expression().release());

        match_or_throw(token_type::BRACKET_R, t, "Expected closing ')' but was not found!");
        return expr;
    }

    // literal false
    else if (match(token_type::BOOL_TRUE))
        return std::make_unique<ast::literal>(true);

    // literal true
    else if (match(token_type::BOOL_FALSE))
        return std::make_unique<ast::literal>(false);

    // literal null
    else if (match(token_type::KEYWORD_NULL))
        return std::make_unique<ast::literal>();

    else if (match(token_type::INTEGER_LITERAL))
        return std::make_unique<ast::literal>(previous().data.literal_int);

    else if (match(token_type::FLOAT_LITERAL))
        return std::make_unique<ast::literal>(previous().data.literal_float);

    else if (match(token_type::STRING_LITERAL))
        return std::make_unique<ast::literal>(previous().data.string);

    // core type int
    else if (match(token_type::TYPE_INTEGER))
        return std::make_unique<ast::core_type>(previous().type);

    // core type bool
    else if (match(token_type::TYPE_BOOLEAN))
        return std::make_unique<ast::core_type>(previous().type);

    // core type float
    else if (match(token_type::TYPE_FLOAT))
        return std::make_unique<ast::core_type>(previous().type);

    // core type string
    else if (match(token_type::TYPE_STRING))
        return std::make_unique<ast::core_type>(previous().type);

    else if (match(token_type::IDENTIFIER))
        return std::make_unique<ast::identifier>(previous().data.string);

    throw syntax_error(peek(), "Unexpected/invalid token found!");
    return nullptr;
}

} /* namespace cantrip  */
