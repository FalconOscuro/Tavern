#include "cantrip/parser/parser.h"

#include <flib/exception/unimplemented.hpp>

#include "cantrip/ast/expression/binary.h"
#include "cantrip/ast/expression/call.h"
#include "cantrip/ast/expression/cast.h"
#include "cantrip/ast/expression/core_type.h"
#include "cantrip/ast/expression/grouping.h"
#include "cantrip/ast/expression/identifier.h"
#include "cantrip/ast/expression/literal.hpp"
#include "cantrip/ast/expression/type_check.h"
#include "cantrip/ast/expression/unary.h"

namespace cantrip {

parser::expr_ptr parser::expression() {
    return cast();
}

/* binary expression parsing in order of precedence */

#define PARSE_BINARY_EXPR(TOKEN_MATCH, EXPR_TYPE, CALL_NEXT)    \
    expr_ptr expr = CALL_NEXT;                                  \
                                                                \
    while (match(TOKEN_MATCH)) {                                \
        expr = std::make_unique<ast::binary>(                   \
                previous().pos,                                 \
                ast::EXPR_TYPE,                                 \
                expr.release(), CALL_NEXT.release()             \
        );                                                      \
    }                                                           \
                                                                \
    return expr

parser::expr_ptr parser::cast() {

    auto expr = logical_or();

    if (match(KEYWORD_AS))
    {
        std::unique_ptr<ast::cast> cast;

        // core_type
        if (safe_peek_compare({TYPE_INTEGER, TYPE_FLOAT, TYPE_BOOLEAN, TYPE_STRING, TYPE_ENTITY}))
            cast = std::make_unique<ast::cast>(previous().pos, peek().type);

        else if (safe_peek_compare(IDENTIFIER))
            cast = std::make_unique<ast::cast>(previous().pos, peek().data.string);

        else
            throw error::syntax(peek(), "is not a valid core type or identifier for type casting");

        cast->expr.reset(expr.release());
        ++m_index;

        expr.reset(cast.release());
    }

    return expr;
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

// TODO: requires same treatement as cast
parser::expr_ptr parser::is()
{
    expr_ptr expr = assign();


    if (match(KEYWORD_IS))
    {
        std::unique_ptr<ast::type_check> type_check;

        // core_type
        if (safe_peek_compare({TYPE_INTEGER, TYPE_FLOAT, TYPE_BOOLEAN, TYPE_STRING, TYPE_ENTITY}))
            type_check = std::make_unique<ast::type_check>(previous().pos, peek().type);

        else if (safe_peek_compare(IDENTIFIER))
            type_check = std::make_unique<ast::type_check>(previous().pos, peek().data.string);

        else
            throw error::syntax(peek(), "is not a valid core type or identifier for type checking");

        type_check->expr.reset(expr.release());
        ++m_index;

        expr.reset(type_check.release());
    }

    return expr;
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
    if (match(BOOL_NOT)) {
        return std::make_unique<ast::unary>(
            previous().pos,
            ast::LOGICAL_NOT,
            logical_not().release()
        );
    }

    return is_equal();
}

parser::expr_ptr parser::sign()
{
    if (match(SUBTRACT)) {
        return std::make_unique<ast::unary>(
            previous().pos,
            ast::MINUS,
            sign().release()
        );
    }

    return is();
}

parser::expr_ptr parser::call()
{
    expr_ptr expr = nullptr;

    // branch here to allow for constructor/public function calls
    // NOTE: Prevents builtin types from having constructors
    if (safe_peek_compare({IDENTIFIER, BRACKET_L}))
    {
        u_ptr<ast::call> call = std::make_unique<ast::call>(
            previous().pos,
            peek().data.string
        );

        m_index += 2;
        call->params = call_param_list();
        call->params.shrink_to_fit();
        expr = expr_ptr(call.release());
    }

    // standard recursive descent
    else
        expr = subscription();

    // handling of object member function calls
    while (safe_peek_compare({DOT, IDENTIFIER, BRACKET_L}))
    {
        u_ptr<ast::call> call = std::make_unique<ast::call>(
            previous().pos,
            peek(1).data.string,
            expr.release()
        );

        m_index += 3;
        call->params = call_param_list();
        call->params.shrink_to_fit();
        expr = expr_ptr(call.release());
    }

    return expr;
}

std::vector<parser::expr_ptr> parser::call_param_list()
{
    std::vector<expr_ptr> params;
    const token& t = previous();

    if (safe_peek_compare(BRACKET_R)) {
        do {
            params.push_back(expression());
        } while (match(COMMA));
    }

    match_or_throw(BRACKET_R, t, "Failed to find closing ')'.");
    return params;
}

parser::expr_ptr parser::subscription()
{
    expr_ptr expr = primary();

    if (match(BRACKET_SQUARE_L))
    {
        const token& t = previous();

        expr = std::make_unique<ast::binary>(
            t.pos,
            ast::SUBSCRIPTION,
            expr.release(),
            expression().release()
        );

        match_or_throw(BRACKET_SQUARE_R, t, "Could not find closing ']'.");
    }

    return expr;
}

parser::expr_ptr parser::primary()
{
    // NOTE: Need to check for unexpected file end?

    const token& t = peek();

    // grouping
    if (match(BRACKET_L))
    {
        expr_ptr expr = std::make_unique<ast::grouping>(
            t.pos,
            expression().release()
        );

        match_or_throw(BRACKET_R, t, "Expected closing ')' but was not found!");
        return expr;
    }

    // literal false
    else if (match(BOOL_TRUE))
        return std::make_unique<ast::literal>(t.pos, true);

    // literal true
    else if (match(BOOL_FALSE))
        return std::make_unique<ast::literal>(t.pos, false);

    // literal null
    else if (match(KEYWORD_NULL))
        return std::make_unique<ast::literal>(t.pos);

    else if (match(INTEGER_LITERAL)) {
        return std::make_unique<ast::literal>(
            t.pos,
            previous().data.literal_int
        );
    }

    else if (match(FLOAT_LITERAL)) {
        return std::make_unique<ast::literal>(
            t.pos,
            previous().data.literal_float
        );
    }

    else if (match(STRING_LITERAL)) {
        return std::make_unique<ast::literal>(
            t.pos,
            previous().data.string
        );
    }

    // core types
    else if (safe_peek_compare({ TYPE_INTEGER, TYPE_FLOAT, TYPE_STRING, TYPE_BOOLEAN, TYPE_ENTITY }))
    {
        auto expr = std::make_unique<ast::core_type>(
            t.pos,
            previous().type
        );
        m_index++;
        return expr;
    }

    else if (match(IDENTIFIER)) {
        return std::make_unique<ast::identifier>(
            t.pos,
            previous().data.string
        );
    }

    throw error::syntax(peek(), "Unexpected/invalid token found!");
    return nullptr;
}

} /* namespace cantrip  */
