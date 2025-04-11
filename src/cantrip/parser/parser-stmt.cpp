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

    if (match(token::COLON))
        return block_implicit();

    else if (match(token::BLOCK_START))
        return block_explicit();

    // variable declare is identifier/core_type followed by identifier
    else if (peek_is_var_declare()) {
        stmt_ptr stmt = var_declare();
        match_stmt_end();
        return stmt;
    }

    else if (match(token::IF))
        return if_else();

    // syntax error for orphaned elif and else statments
    else if (match(token::ELSE_IF) || match(token::ELSE))
        throw syntax_error(previous(), "No preceeding if or elif statement");

    else if (match(token::WHILE))
        return while_stmt();

    else if (match(token::RETURN))
        return return_stmt();

    else if (match(token::BREAK) || match(token::CONTINUE))
        return flow();

    else if (match(token::FUNCTION))
        return function();

    else if (match(token::COMPONENT))
        return component();

    return expression_statement();
}

ast::var_declare* parser::var_declare()
{
    const token& t_type = peek();
    const char* type;

    if (t_type == token::IDENTIFIER)
        type = t_type.data.string;

    else
        type = get_token_coretype_name(t_type.ttype);

    ast::var_declare* stmt = new ast::var_declare(type, next().data.string);
    m_index += 2;

    if (match(token::ASSIGN))
        stmt->expr = expression();

    return stmt;
}

parser::stmt_ptr parser::block_implicit()
{
    match_or_throw(token::NEW_LINE, previous(), "Expected newline after ':' at implicit block start");

    // indent level of outer code portion
    const auto outer_indent = previous().pos.indent;

    ast::block* block = new ast::block();

    // loop so long as indent level is greater than outer code block
    while (peek().pos.indent > outer_indent) {
        block->stmts.push_back(statement());
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
    ast::block* block = new ast::block();

    // loop until block end
    while (!match(token::BLOCK_END)) {
        // error if file ends before block
        if (at_end())
            throw syntax_error(peek(), "Expected explicit block end '}' but found file end instead");

        block->stmts.push_back(statement());
    }

    // prevent empty code blocks
    if (block->stmts.empty())
        throw syntax_error(previous(), "Empty blocks are unsupported!");

    return block;
}

parser::stmt_ptr parser::if_else()
{
    ast::if_else* stmt = new ast::if_else();

    stmt->condition = expect_expression();
    stmt->exec_stmt = statement();

    if (match(token::ELSE_IF))
        stmt->else_stmt = if_else();

    else if (match(token::ELSE))
        stmt->else_stmt = statement();

    return stmt;
}

parser::stmt_ptr parser::while_stmt()
{
    ast::while_stmt* stmt = new ast::while_stmt();

    stmt->condition = expect_expression();
    stmt->exec_stmt = statement();
    return stmt;
}

parser::stmt_ptr parser::return_stmt()
{
    ast::return_stmt* stmt = new ast::return_stmt();

    if (!(peek() == token::STATEMENT_END || peek() == token::NEW_LINE))
        stmt->returned = expect_expression();

    match_stmt_end();
    return stmt;
}

parser::stmt_ptr parser::flow()
{
    stmt_ptr stmt = new ast::flow(previous() == token::CONTINUE);

    match_stmt_end();
    return stmt;
}

ast::function* parser::function()
{
    ast::function* stmt = nullptr;
    const token& t_func = previous();
    const token& t_name = peek();

    match_or_throw(token::IDENTIFIER, t_func, "Expected identifier after function keyword.");

    match_or_throw(token::BRACKET_L, t_func, "Expected to find '(' character for function paramaters.");

    std::vector<ast::var_declare*> params;

    if (peek() != token::BRACKET_R) {
        do {
            if (!peek_is_var_declare())
                throw syntax_error(peek(), "Expected function paramater declaration.");

            params.push_back(var_declare());
        } while(match(token::COMMA));
    }

    match_or_throw(token::BRACKET_R, peek(), "Could not find closing ')' for function paramater list.");

    // function return type defined with '>'
    // func IDENTIFIER() [> [CORE_TYPE_* | IDENTIFIER | KEYWORD_NULL]
    if (match(token::GREATER_THAN))
    {
        const token& t_type = peek();

        if (peek_is_type()) {
            stmt = new ast::function(t_name.data.string, get_token_coretype_name(t_type.ttype));

            m_index++;
        }

        else if (t_type == token::KEYWORD_NULL) {
            m_index++;
        }

        else
            throw syntax_error(t_type, "Expected type or null for function return type.");
    }

    // not fall through else as "> null" is same as no defined return type
    if (stmt == nullptr)
        stmt = new ast::function(t_name.data.string);

    // WARNING: Allows for nested functions!
    stmt->params = params;
    stmt->body = statement();

    return stmt;
}

ast::component* parser::component()
{
    const token& t_name = peek();
    match_or_throw(token::IDENTIFIER, t_name, "Expected component name.");

    ast::component* stmt = new ast::component(t_name.data.string);

    // component body only accepts function and variable declarations,
    // requires different handling as cannot just use block statement

    flib::uint_32 outer_indent = previous().pos.indent;
    flib::uint_32 inner_indent = 0;

    bool block_implicit = false;

    if (match(token::COLON) && match(token::NEW_LINE)) {
        block_implicit = true;
        inner_indent = peek().pos.indent;

        if (inner_indent <= outer_indent)
            throw syntax_error(peek(), "Body of implicit block should have greater indent than the outer body.");
    }

    else if (!match(token::BLOCK_START))
        throw syntax_error(peek(), "Expected block start after component declaration.");

    do
    {
        discard_tokens();

        if (peek_is_var_declare()) {
            stmt->vars.push_back(var_declare());
            match_stmt_end();
        }

        if (match(token::FUNCTION))
            stmt->funcs.push_back(function());

    } while (peek() != token::FILE_END
                && ((!block_implicit && !match(token::BLOCK_END))
             || (block_implicit && peek().pos.indent >= inner_indent)));

    if (!block_implicit && previous() != token::BLOCK_END)
        throw syntax_error(peek(), "Expected could not find enclosing '}' for block.");

    return stmt;
}

parser::stmt_ptr parser::expression_statement()
{
    stmt_ptr stmt = new ast::expr_stmt(expression());

    match_stmt_end();

    return stmt;
}

} /* namespace cantrip */
