#include "cantrip/parser/parser.h"

#include "cantrip/ast/expression/expression.h"

#include "cantrip/ast/statement/block.h"
#include "cantrip/ast/statement/expr_stmt.h"
#include "cantrip/ast/statement/flow.h"
#include "cantrip/ast/statement/for_stmt.h"
#include "cantrip/ast/statement/function.h"
#include "cantrip/ast/statement/if_else.h"
#include "cantrip/ast/statement/return_stmt.h"
#include "cantrip/ast/statement/var_declare.h"
#include "cantrip/ast/statement/while_stmt.h"

#include "cantrip/error/semantic_error.h"

namespace cantrip {

parser::stmt_ptr parser::statement()
{
    discard_tokens();

    parser::stmt_ptr stmt;
    const file_pos pos = peek().pos;

    if (match(COLON))
        stmt = block_implicit();

    else if (match(BLOCK_START))
        stmt = block_explicit();

    // variable declare is identifier/core_type followed by identifier
    else if (peek_is_var_declare()) {
        stmt = var_declare();
        match_stmt_end();
    }

    else if (match(IF))
        stmt = if_else();

    // syntax error for orphaned elif and else statments
    else if (match(ELSE_IF) || match(ELSE))
        throw error::syntax(previous(), "No preceeding if or elif statement");

    else if (match(WHILE))
        stmt = while_stmt();

    else if (match(RETURN))
        stmt = return_stmt();

    else if (match(BREAK) || match(CONTINUE))
        stmt = flow();

    else if (match(FUNCTION))
        throw error::syntax(previous(), "Function definitions only supported at global scope or within data structures");

    else if (match(COMPONENT))
        throw error::syntax(previous(), "Component definitions only supported at global scope");
    // repeat above for classes, structs & systems?

    else
        stmt = expression_statement();

    stmt->pos = pos;
    return stmt;
}

parser::u_ptr<ast::var_declare> parser::var_declare()
{
    const token& t = peek();
    u_ptr<ast::var_declare> stmt = nullptr;

    if (t == IDENTIFIER)
        stmt = std::unique_ptr<ast::var_declare>(new ast::var_declare(t.data.string, peek(1).data.string));

    else
        stmt = std::unique_ptr<ast::var_declare>(new ast::var_declare(t.type, peek(1).data.string));

    m_index += 2;
    // can get overwritten, but easy hack to account for params
    stmt->pos = t.pos;

    // array type
    // no multi dimensional arrays, or dynamic lists yet
    if (match(BRACKET_SQUARE_L))
    {
        const token& t_bracket_l = previous();
        match_or_throw(INTEGER_LITERAL, t_bracket_l, "Expected integer literal for var declare array size");

        // technically not a syntax error, but works fine for now
        if ((stmt->vtype.array_size = previous().data.literal_int) == 0)
            throw error::syntax(previous(), "Arrays of size 0 are not valid!");

        match_or_throw(BRACKET_SQUARE_R, t_bracket_l, "Failed to find closing bracket!");
    }

    if (match(ASSIGN))
    {
        if (stmt->vtype.array_size > 0)
            throw error::syntax(previous(), "Declaration assignment for arrays is currently unsupported!");

        stmt->expr = expression();
    }

    return stmt;
}

parser::stmt_ptr parser::block_implicit()
{
    // indent level of outer code portion
    const file_pos pos = previous().pos;

    match_or_throw(NEW_LINE, previous(), "Expected newline after ':' at implicit block start");

    u_ptr<ast::block> block = std::make_unique<ast::block>();

    // loop so long as indent level is greater than outer code block
    while (peek().pos.indent > pos.indent) {
        block->stmts.emplace_back(statement());
    }

    // prevent empty code blocks
    if (block->stmts.empty()) {
        throw error::syntax(peek(),
            "Implicit blocks must contain at least one(1) statement and increase the indent level by at least one(1)"
        );
    }

    block->stmts.shrink_to_fit();
    return block;
}

parser::stmt_ptr parser::block_explicit()
{
    u_ptr<ast::block> block = std::make_unique<ast::block>();

    // loop until block end
    while (!match(BLOCK_END)) {
        // error if file ends before block
        if (at_end())
            throw error::syntax(peek(), "Expected explicit block end '}' but found file end instead");

        block->stmts.emplace_back(statement());
    }

    // prevent empty code blocks
    if (block->stmts.empty())
        throw error::syntax(previous(), "Empty blocks are unsupported!");

    block->stmts.shrink_to_fit();
    return block;
}

parser::stmt_ptr parser::if_else()
{
    u_ptr<ast::if_else> stmt = std::make_unique<ast::if_else>();

    stmt->condition = expect_expression();
    stmt->exec_stmt = statement();

    if (match(ELSE_IF))
        stmt->else_stmt = if_else();

    else if (match(ELSE))
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

parser::stmt_ptr parser::for_stmt()
{
    u_ptr<ast::for_stmt> stmt = std::make_unique<ast::for_stmt>();
    const token& t_for = previous();

    if (!peek_is_var_declare())
        throw  error::syntax(t_for, "Expected for statement iterator var declaration");

    stmt->iterator_var = var_declare();

    match_or_throw(KEYWORD_IN, t_for, "Expected in keyword after for statement iterator var declaration");

    stmt->loop_expr = expect_expression();
    stmt->exec_stmt = statement();
    return stmt;
}

parser::stmt_ptr parser::return_stmt()
{
    u_ptr<ast::return_stmt> stmt = std::make_unique<ast::return_stmt>();

    // NOTE: Should be doing safe peek here
    if (!(peek() == STATEMENT_END || peek() == NEW_LINE))
        stmt->returned = expect_expression();

    match_stmt_end();
    return stmt;
}

parser::stmt_ptr parser::flow()
{
    stmt_ptr stmt = std::make_unique<ast::flow>(previous() == CONTINUE);

    match_stmt_end();
    return stmt;
}

parser::u_ptr<ast::component> parser::component()
{
    const token& t_comp = previous();
    const token& t_name = peek();
    match_or_throw(IDENTIFIER, t_name, "Expected component name.");

    u_ptr<ast::component> stmt = std::make_unique<ast::component>(t_name.data.string);

    // component body only accepts function and variable declarations,
    // requires different handling as cannot just use block statement

    uint32_t outer_indent = previous().pos.indent;
    uint32_t inner_indent = 0;

    bool block_implicit = false;

    if (match(COLON) && match(NEW_LINE)) {
        block_implicit = true;
        inner_indent = peek().pos.indent;

        if (inner_indent <= outer_indent)
            throw error::syntax(peek(), "Body of implicit block should have greater indent than the outer body.");
    }

    else {
        discard_tokens();
        match_or_throw(BLOCK_START, t_comp, "Expected block start after component declaration.");
    }

    do
    {
        discard_tokens();
        const token& t_member = peek();

        if (peek_is_var_declare())
        {
            u_ptr<ast::var_declare> var = var_declare();

            if (!stmt->try_add_var(var))
                throw error::redefinition(var.get());

            match_stmt_end();
        }

        else if (match(FUNCTION))
        {
            u_ptr<ast::function> func = function();
            func->pos = t_member.pos;

            if (!stmt->try_add_func(func))
                throw error::redefinition(func.get());
        }

    } while (!at_end()
                && ((!block_implicit && !match(BLOCK_END))
             || (block_implicit && peek().pos.indent >= inner_indent)));

    if (!block_implicit && previous() != BLOCK_END)
        throw error::syntax(peek(), "Expected could not find enclosing '}' for block.");

    return stmt;
}

parser::u_ptr<ast::function> parser::function()
{
    u_ptr<ast::function> stmt = nullptr;
    const token& t_func = previous();
    const token& t_name = peek();
    const char* return_type = nullptr;

    match_or_throw(IDENTIFIER, t_func, "Expected identifier after function keyword.");
    match_or_throw(BRACKET_L, t_func, "Expected to find '(' character for function paramaters.");

    using param_vec = std::vector<u_ptr<ast::var_declare>>;
    param_vec params;

    if (!safe_peek_compare(BRACKET_R)) {
        do {
            if (!peek_is_var_declare())
                throw error::syntax(peek(), "Expected function paramater declaration.");

            u_ptr<ast::var_declare> param = var_declare();
            if (param->expr)
                throw error::syntax(previous(), "Default parameter assignment for functions is currently unsupported.");

            params.emplace_back(param.release());
        } while(match(COMMA));
    }

    match_or_throw(BRACKET_R, peek(), "Could not find closing ')' for function paramater list.");

    // function return type defined with '->'
    // func IDENTIFIER() [> [CORE_TYPE_* | IDENTIFIER | KEYWORD_NULL]
    // TODO: Safe compare
    if (match(ARROW))
    {
        const token& t_type = peek();

        if (peek_is_type()) {
            return_type = get_token_type_name(t_type);
            m_index++;
        }

        else if (t_type == KEYWORD_NULL) {
            m_index++;
        }

        else
            throw error::syntax(t_type, "Expected type or null for function return type.");
    }

    stmt = std::make_unique<ast::function>(t_name.data.string, return_type);

    // move construct should work?
    params.shrink_to_fit();
    stmt->params = param_vec(std::move(params));

    stmt->body = statement();

    return stmt;
}

parser::u_ptr<ast::system> parser::system()
{
    const token& t_sys  = previous();
    const token& t_name = peek();

    match_or_throw(IDENTIFIER, t_sys, "Expected identifier after system keyword.");
    match_or_throw(BRACKET_L, t_sys, "Expected to find '(' character for system parameters.");

    auto stmt = std::make_unique<ast::system>(t_name.data.string);

    if (safe_peek_compare(BRACKET_R))
        throw error::syntax(peek(), "System parameter list cannot be empty!");

    do {
        if (!peek_is_var_declare())
            throw error::syntax(peek(), "Expected system parameter declaration.");

        u_ptr<ast::var_declare> param = var_declare();
        if (param->expr)
            throw error::syntax(previous(), "System parameters do not support default assignment!");

        else if (param->vtype.array_size > 0)
            throw error::syntax(previous(), "System parameters cannot be arrays!");

        stmt->params.emplace_back(param.release());
    } while(match(COMMA));

    match_or_throw(BRACKET_R, peek(), "Could not find closing ')' for system parameter list.");

    stmt->params.shrink_to_fit();
    stmt->body = statement();

    return stmt;
}

parser::stmt_ptr parser::expression_statement()
{
    stmt_ptr stmt = std::make_unique<ast::expr_stmt>(expression().release());

    match_stmt_end();

    return stmt;
}

} /* namespace cantrip */
