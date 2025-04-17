#include "cantrip/parser/ast/expression/expression.h"
#include "cantrip/parser/parser.h"

#include <exception>

#include "cantrip/parser/ast/statement/expr_stmt.h"
#include "cantrip/parser/ast/statement/block.h"
#include "cantrip/parser/ast/statement/var_declare.h"
#include "cantrip/parser/ast/statement/if_else.h"
#include "cantrip/parser/ast/statement/while_stmt.h"
#include "cantrip/parser/ast/statement/return_stmt.h"
#include "cantrip/parser/ast/statement/flow.h"
#include "cantrip/parser/ast/statement/function.h"

namespace cantrip {

parser::stmt_ptr parser::statement()
{
    discard_tokens();

    if (match(token_type::COLON))
        return block_implicit();

    else if (match(token_type::BLOCK_START))
        return block_explicit();

    // variable declare is identifier/core_type followed by identifier
    else if (peek_is_var_declare()) {
        stmt_ptr stmt = var_declare();
        match_stmt_end();
        return stmt;
    }

    else if (match(token_type::IF))
        return if_else();

    // syntax error for orphaned elif and else statments
    else if (match(token_type::ELSE_IF) || match(token_type::ELSE))
        throw syntax_error(previous(), "No preceeding if or elif statement");

    else if (match(token_type::WHILE))
        return while_stmt();

    else if (match(token_type::RETURN))
        return return_stmt();

    else if (match(token_type::BREAK) || match(token_type::CONTINUE))
        return flow();

    //else if (match(token::FUNCTION))
    //    return function();

    //else if (match(token::COMPONENT))
    //    return component();

    return expression_statement();
}

parser::u_ptr<ast::var_declare> parser::var_declare()
{
    const token& t_type = peek();
    const char* type;

    if (t_type == token_type::IDENTIFIER)
        type = t_type.data.string;

    else
        type = get_token_type_name(t_type);

    u_ptr<ast::var_declare> stmt =
        std::make_unique<ast::var_declare>(type, next().data.string);

    m_index += 2;

    if (match(token_type::ASSIGN))
        stmt->expr = expression();

    return stmt;
}

parser::stmt_ptr parser::block_implicit()
{
    match_or_throw(token_type::NEW_LINE, previous(), "Expected newline after ':' at implicit block start");

    // indent level of outer code portion
    const auto outer_indent = previous().pos.indent;

    u_ptr<ast::block> block = std::make_unique<ast::block>();

    // loop so long as indent level is greater than outer code block
    while (peek().pos.indent > outer_indent) {
        block->stmts.emplace_back(statement());
    }

    // prevent empty code blocks
    if (block->stmts.empty()) {
        throw syntax_error(peek(),
            "Implicit blocks must contain at least one(1) statement and increase the indent level by at least one(1)"
        );
    }

    return block;
}

parser::stmt_ptr parser::block_explicit()
{
    u_ptr<ast::block> block = std::make_unique<ast::block>();

    // loop until block end
    while (!match(token_type::BLOCK_END)) {
        // error if file ends before block
        if (at_end())
            throw syntax_error(peek(), "Expected explicit block end '}' but found file end instead");

        block->stmts.emplace_back(statement());
    }

    // prevent empty code blocks
    if (block->stmts.empty())
        throw syntax_error(previous(), "Empty blocks are unsupported!");

    return block;
}

parser::stmt_ptr parser::if_else()
{
    u_ptr<ast::if_else> stmt = std::make_unique<ast::if_else>();

    stmt->condition = expect_expression();
    stmt->exec_stmt = statement();

    if (match(token_type::ELSE_IF))
        stmt->else_stmt = if_else();

    else if (match(token_type::ELSE))
        stmt->else_stmt = statement();

    return stmt;
}

parser::stmt_ptr parser::while_stmt()
{
    u_ptr<ast::while_stmt> stmt = std::make_unique<ast::while_stmt>();

    stmt->condition = expect_expression();
    stmt->exec_stmt = statement();
    return stmt;
}

parser::stmt_ptr parser::return_stmt()
{
    u_ptr<ast::return_stmt> stmt = std::make_unique<ast::return_stmt>();

    if (!(peek() == token_type::STATEMENT_END || peek() == token_type::NEW_LINE))
        stmt->returned = expect_expression();

    match_stmt_end();
    return stmt;
}

parser::stmt_ptr parser::flow()
{
    stmt_ptr stmt = std::make_unique<ast::flow>(previous() == token_type::CONTINUE);

    match_stmt_end();
    return stmt;
}

parser::u_ptr<ast::function> parser::function()
{
    u_ptr<ast::function> stmt = nullptr;
    const token& t_func = previous();
    const token& t_name = peek();
    const char* return_type = nullptr;

    match_or_throw(token_type::IDENTIFIER, t_func, "Expected identifier after function keyword.");

    match_or_throw(token_type::BRACKET_L, t_func, "Expected to find '(' character for function paramaters.");

    std::vector<u_ptr<ast::var_declare>> params;

    if (peek() != token_type::BRACKET_R) {
        do {
            if (!peek_is_var_declare())
                throw syntax_error(peek(), "Expected function paramater declaration.");

            params.emplace_back(var_declare());
        } while(match(token_type::COMMA));
    }

    match_or_throw(token_type::BRACKET_R, peek(), "Could not find closing ')' for function paramater list.");

    // function return type defined with '>'
    // func IDENTIFIER() [> [CORE_TYPE_* | IDENTIFIER | KEYWORD_NULL]
    if (match(token_type::GREATER_THAN))
    {
        const token& t_type = peek();

        if (peek_is_type()) {
            return_type = get_token_type_name(t_type);
            m_index++;
        }

        else if (t_type == token_type::KEYWORD_NULL) {
            m_index++;
        }

        else
            throw syntax_error(t_type, "Expected type or null for function return type.");
    }

    stmt = std::make_unique<ast::function>(t_name.data.string, return_type);

    stmt->params.reserve(params.size());
    for (size_t i = 0; i < params.size(); ++i)
        stmt->params.emplace_back(params[i].release());

    params.clear();

    stmt->body = statement();

    return stmt;
}

parser::u_ptr<ast::component> parser::component()
{
    const token& t_name = peek();
    match_or_throw(token_type::IDENTIFIER, t_name, "Expected component name.");

    u_ptr<ast::component> stmt = std::make_unique<ast::component>(t_name.data.string);

    // component body only accepts function and variable declarations,
    // requires different handling as cannot just use block statement

    uint32_t outer_indent = previous().pos.indent;
    uint32_t inner_indent = 0;

    bool block_implicit = false;

    if (match(token_type::COLON) && match(token_type::NEW_LINE)) {
        block_implicit = true;
        inner_indent = peek().pos.indent;

        if (inner_indent <= outer_indent)
            throw syntax_error(peek(), "Body of implicit block should have greater indent than the outer body.");
    }

    else if (!match(token_type::BLOCK_START))
        throw syntax_error(peek(), "Expected block start after component declaration.");

    do
    {
        discard_tokens();

        if (peek_is_var_declare()) {
            stmt->vars.push_back(var_declare());
            match_stmt_end();
        }

        if (match(token_type::FUNCTION))
            stmt->funcs.push_back(function());

    } while (peek() != token_type::FILE_END
                && ((!block_implicit && !match(token_type::BLOCK_END))
             || (block_implicit && peek().pos.indent >= inner_indent)));

    if (!block_implicit && previous() != token_type::BLOCK_END)
        throw syntax_error(peek(), "Expected could not find enclosing '}' for block.");

    return stmt;
}

parser::stmt_ptr parser::expression_statement()
{
    stmt_ptr stmt = std::make_unique<ast::expr_stmt>(expression().release());

    match_stmt_end();

    return stmt;
}

} /* namespace cantrip */
